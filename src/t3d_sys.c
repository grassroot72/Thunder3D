/*----- t3d_sys.c ------------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_util.h>
#include <t3d_timer.h>
#include <t3d_math.h>
#include <t3d_vector.h>
#include <t3d_hashtable.h>
#include <t3d_slist.h>
#include <t3d_texture.h>
#include <t3d_object.h>
#include <t3d_camera.h>
#include <t3d_ms3d.h>
#include <t3d_decal.h>
#include <t3d_aamesh.h>
#include <t3d_unit.h>
#include <t3d_heightmap.h>
#include <t3d_patchmap.h>
#include <t3d_pick.h>
#include <t3d_sys.h>


/* load textures from a file which contains all the texture names */
static hashtable *
sys_texdb_new( const char *listfile )
{
  int i;
  char line[256];

  vector *v = vector_new(sizeof(line));
  util_file_to_vector(v, listfile);

  hashtable *texdb = hash_new(v->size);
  GLuint *tex_ids = (GLuint *)malloc(sizeof(GLuint)*v->size);

  for(i = 0; i < v->size; i++) {
    char *texinfo = vector_at(v, i);    /* texinfo has the directory information */

    char path[256];
    int s3tc_compressed;
    sscanf(texinfo, "%s %d", path, &s3tc_compressed);
    tex_ids[i] = tex_load(path, s3tc_compressed);

    util_add_basename_to_hash(texdb, path, &tex_ids[i], sizeof(GLuint));
  }

  free(tex_ids);
  vector_del(v);
  return texdb;
}

/* delete texture db from memory */
static void
sys_texdb_del( hashtable *texdb )
{
  GLuint tex_id = 1;

  int i;
  for(i = 0; i < texdb->nchains; i++) {
    glDeleteTextures(1, &tex_id);
    tex_id++;
  }

  hash_del(texdb);
}

/* load models from a file which contains all the model names */
static ms3d **
sys_models_new( unsigned int *n_models, const hashtable *texdb, const char *listfile )
{
  int i;
  char line[512];

  vector *v = vector_new(sizeof(line));
  util_file_to_vector(v, listfile);

  ms3d **models = (ms3d **)malloc(sizeof(ms3d *)*v->size);

  for(i = 0; i < v->size; i++) {
    char *modelinfo = vector_at(v, i);     /* modelinfo has the directory information */

    char path[256];
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;
    sscanf(modelinfo, "%s %f %f %f %f %f %f", path, &min_x, &min_y, &min_z, &max_x, &max_y, &max_z);

    unsigned char *buf = (unsigned char *)util_file_to_mem(path);
    models[i] = ms3d_new(buf, texdb);
    vec3_set(&models[i]->aabb_min, min_x, min_y, min_z);
    vec3_set(&models[i]->aabb_max, max_x, max_y, max_z);

    float len_x = max_x - min_x;
    float len_y = max_z - min_z;
    models[i]->radius = sqrtf(len_x*len_x + len_y*len_y)*0.5;

    free(buf);

    /* load ms3d animation */
    strcat(path, ".anim");
    vector *anicmd_strs = vector_new(sizeof(line));
    util_file_to_vector(anicmd_strs, path);
    models[i]->anim_cmds = ms3d_animcmd_new(&models[i]->n_cmds, anicmd_strs);
    vector_del(anicmd_strs);
  }

  *n_models = v->size;

  vector_del(v);
  return models;
}

/* delete the loaded ms3d model from memory */
static void
sys_models_del( ms3d **models, const unsigned int n_models )
{
  unsigned int i;

  for(i = 0; i < n_models; i++) {
    ms3d_del(models[i]);
  }
}

/* load ms3d model names from a file */
static hashtable *
sys_modeldb_new( const char *listfile )
{
  int i;
  char line[256];

  vector *v = vector_new(sizeof(line));
  util_file_to_vector(v, listfile);

  hashtable *modeldb = hash_new(v->size);
  unsigned int *model_ids = (unsigned int *)malloc(sizeof(unsigned int)*v->size);

  for(i = 0; i < v->size; i++) {
    char *modelinfo = vector_at(v, i);  /* modelinfo has the directory information */

    char path[256];
    sscanf(modelinfo, "%s", path);
    model_ids[i] = i;

    util_add_basename_to_hash(modeldb, path, &model_ids[i], sizeof(unsigned int));
  }

  free(model_ids);
  vector_del(v);
  return modeldb;
}

/* delete model db from memory */
static void
sys_modeldb_del( hashtable *modeldb )
{
  hash_del(modeldb);
}

/* create a new system struct in memory */
t3dsys *
sys_new( void )
{
  t3dsys *sys = (t3dsys *)malloc(sizeof(t3dsys));

  vec4_set(&sys->colors[0], 1.0, 0.0, 0.0, 1.0);  /* red */
  vec4_set(&sys->colors[1], 0.0, 1.0, 0.0, 1.0);  /* green */
  vec4_set(&sys->colors[2], 0.0, 0.0, 1.0, 1.0);  /* blue */
  vec4_set(&sys->colors[3], 1.0, 1.0, 1.0, 1.0);  /* white */
  vec4_set(&sys->colors[4], 0.0, 0.0, 0.0, 1.0);  /* black */
  vec4_set(&sys->colors[5], 1.0, 1.0, 0.0, 1.0);  /* yellow */
  vec4_set(&sys->colors[6], 0.0, 1.0, 1.0, 1.0);  /* cyan */
  vec4_set(&sys->colors[7], 1.0, 0.0, 1.0, 1.0);  /* pink */

  /* create texture database */
  sys->texdb = sys_texdb_new("texture_list.txt");

  /* create patchmap */
  sys->pchmap = pchmap_new("maps/terrain.png", 9, 9, 25.0, 1.0, sys->texdb);

  /* create ms3d models */
  sys->models = sys_models_new(&sys->n_models, sys->texdb, "model_list.txt");
  sys->modeldb = sys_modeldb_new("model_list.txt");


  patchmap *pchmap = sys->pchmap;
  /* create the decal o_aamsh of the ms3d models */
  int i;
  for(i = 0; i < sys->n_models; i++) {
    ms3d *model = sys->models[i];
    model->o_aamsh = decal_new(model->radius, pchmap->step);
  }

  /* create units */
  sys->units = list_new();
  const unsigned int *model_id;
  vec3 u_pos, u_lookat, forward;

  vec3_set(&u_pos, 358.0, 0.0, 358.0);
  vec3_set(&u_lookat, 158.0, 0.0, 588.0);
  model_id = hash_get(sys->modeldb, "beast.ms3d");
  unit *u0 = unit_new(sys->models, *model_id);
  ms3d_set_anim(u0->ani, u0->model, 'N');
  unit_set_skin(u0, 2, 3, sys->texdb);
  u0->color = CLR_RED;
  unit_set_pos(u0, &u_pos, pchmap);
  obj_find_f_axis(&forward, &u_pos, &u_lookat);
  unit_set_dir(u0, &forward);
  ivec2_cpy(&u0->itarget, &u0->ipos);
  aamesh_set_tex(u0->aamsh, sys->texdb);
  aamesh_get_mesh(u0->aamsh, u0->model->o_aamsh, u0->ipos.x, u0->ipos.y, pchmap);
  aamesh_tex_offset(u0->aamsh, u0->model->o_aamsh, u0->pos.x, u0->pos.z, pchmap->step);
  u0->move_speed = 0.118;
  list_push_back(sys->units, list_node_new(u0));

  vec3_set(&u_pos, 280.0, 0.0, 518.0);
  vec3_set(&u_lookat, 58.0, 0.0, 280.0);
  model_id = hash_get(sys->modeldb, "dwarf.ms3d");
  unit *u1 = unit_new(sys->models, *model_id);
  ms3d_set_anim(u1->ani, u1->model, 'N');
  unit_set_skin(u1, 0, 6, sys->texdb);
  u1->color = CLR_GREEN;
  unit_set_pos(u1, &u_pos, pchmap);
  obj_find_f_axis(&forward, &u_pos, &u_lookat);
  unit_set_dir(u1, &forward);
  ivec2_cpy(&u1->itarget, &u1->ipos);
  aamesh_set_tex(u1->aamsh, sys->texdb);
  aamesh_get_mesh(u1->aamsh, u1->model->o_aamsh, u1->ipos.x, u1->ipos.y, pchmap);
  aamesh_tex_offset(u1->aamsh, u1->model->o_aamsh, u1->pos.x, u1->pos.z, pchmap->step);
  u1->move_speed = 0.08;
  list_push_back(sys->units, list_node_new(u1));

  vec3_set(&u_pos, 378.0, 0.0, 758.0);
  vec3_set(&u_lookat, 180.0, 0.0, 388.0);
  model_id = hash_get(sys->modeldb, "beast.ms3d");
  unit *u2 = unit_new(sys->models, *model_id);
  ms3d_set_anim(u2->ani, u2->model, 'N');
  unit_set_skin(u2, 3, 3, sys->texdb);
  u2->color = CLR_BLUE;
  unit_set_pos(u2, &u_pos, pchmap);
  obj_find_f_axis(&forward, &u_pos, &u_lookat);
  unit_set_dir(u2, &forward);
  ivec2_cpy(&u2->itarget, &u2->ipos);
  aamesh_set_tex(u2->aamsh, sys->texdb);
  aamesh_get_mesh(u2->aamsh, u2->model->o_aamsh, u2->ipos.x, u2->ipos.y, pchmap);
  aamesh_tex_offset(u2->aamsh, u2->model->o_aamsh, u2->pos.x, u2->pos.z, pchmap->step);
  u2->move_speed = 0.118;
  list_push_back(sys->units, list_node_new(u2));

  vec3_set(&u_pos, 278.0, 0.0, 638.0);
  vec3_set(&u_lookat, 380.0, 0.0, 380.0);
  model_id = hash_get(sys->modeldb, "dwarf.ms3d");
  unit *u3 = unit_new(sys->models, *model_id);
  ms3d_set_anim(u3->ani, u3->model, 'N');
  unit_set_skin(u3, 1, 6, sys->texdb);
  u3->color = CLR_YELLOW;
  unit_set_pos(u3, &u_pos, pchmap);
  obj_find_f_axis(&forward, &u_pos, &u_lookat);
  unit_set_dir(u3, &forward);
  ivec2_cpy(&u3->itarget, &u3->ipos);
  aamesh_set_tex(u3->aamsh, sys->texdb);
  aamesh_get_mesh(u3->aamsh, u3->model->o_aamsh, u3->ipos.x, u3->ipos.y, pchmap);
  aamesh_tex_offset(u3->aamsh, u3->model->o_aamsh, u3->pos.x, u3->pos.z, pchmap->step);
  u3->move_speed = 0.08;
  list_push_back(sys->units, list_node_new(u3));
  /* end of create units */

  sys->timer.start = (float)tmr_gettime();

  return sys;
}

static void
sys_listnode_unit_del( listnode *node )
{
  unit_del((unit *)node->data);
}

/* delete system sys from memory */
void
sys_del( t3dsys *sys )
{
  if(!sys) return;

  pchmap_del(sys->pchmap);

  list_del(sys->units, sys_listnode_unit_del);

  sys_models_del(sys->models, sys->n_models);
  sys_modeldb_del(sys->modeldb);

  sys_texdb_del(sys->texdb);

  free(sys);
}