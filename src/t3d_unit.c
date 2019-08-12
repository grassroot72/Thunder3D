/*----- t3d_unit.c -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_util.h>
#include <t3d_geomath.h>
#include <t3d_hashtable.h>
#include <t3d_texture.h>
#include <t3d_ms3d.h>
#include <t3d_ms3dmesh.h>
#include <t3d_mesh.h>
#include <t3d_object.h>
#include <t3d_light.h>
#include <t3d_bbox.h>
#include <t3d_heightmap.h>
#include <t3d_patchmap.h>
#include <t3d_aamesh.h>
#include <t3d_astar.h>
#include <t3d_unit.h>


/* load the model data */
static void
unit_load_ms3d( unit *u, ms3d **models, const unsigned int model_id )
{
  u->model = models[model_id];
  ms3d *model = u->model;

  u->mshs = (mesh **)malloc(sizeof(mesh *)*model->n_mshs);

  int i;
  for(i = 0; i < model->n_mshs; i++) {
    ms3dmesh *o_msh = model->o_mshs[i];

    u->mshs[i] = mesh_new(o_msh->n_vertices);
    mesh *msh = u->mshs[i];

    msh->v_data_size = o_msh->n_vertices*sizeof(vec3);
  }
}

/* store vertices, normals and/or elements in graphic card buffers */
void
unit_upload( const unit *u )
{
  int i, data_size;

  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    /* vertices, normals, tangents */
    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_coords);
    glBufferData(GL_ARRAY_BUFFER, msh->v_data_size, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, msh->v_data_size, NULL, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_tangents);
    glBufferData(GL_ARRAY_BUFFER, msh->v_data_size, NULL, GL_DYNAMIC_DRAW);

    /* hsr indices for camera */
    data_size = sizeof(unsigned short)*msh->n_faces*3;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_cam_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);

    /* hsr indices for light */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_lit_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);
  }
}

/* create unit struct in memory */
unit *
unit_new( ms3d **models, const unsigned int model_id )
{
  unit *u = (unit *)malloc(sizeof(unit));
  if(!u)  return NULL;

  unit_load_ms3d(u, models, model_id);
  u->mat_joint_finals = (mat4 *)malloc(sizeof(mat4)*u->model->n_joints);
  u->ani = ms3d_anim_new(u->model);

  u->picked = 0;

  aabb_calc_size(&u->center,
                 &u->half_x_len,
                 &u->half_y_len,
                 &u->half_z_len,
                 &u->model->aabb_min,
                 &u->model->aabb_max);

  u->aamsh = aamesh_new(u->model->o_aamsh);

  u->path = NULL;
  u->chk_path = 0;
  u->n_steps = 0;

  unit_upload(u);

  return u;
}

/* destroy unit struct */
void
unit_del( unit *u )
{
  if(!u) return;

  free(u->mat_joint_finals);
  ms3d_anim_del(u->ani);

  int i;
  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];
    mesh_del(msh);
  }
  free(u->mshs);

  aamesh_del(u->aamsh);

  free(u);
}

/* set unit's position and target */
void
unit_set_pos( unit *u, const vec3 *pos, patchmap *pm )
{
  int c_index;
  u->ipos.x = (int)(pos->x/pm->step);
  u->ipos.y = (int)(pos->z/pm->step);
  c_index = u->ipos.y*pm->w_cells + u->ipos.x;

  pthread_mutex_lock(&pm->lock);
  pm->cells[c_index].status = ASTAR_AVAIL;
  pthread_mutex_unlock(&pm->lock);

  /* let's find out the vertex indice of a block(2 triangles) the unit is in */
  int i0 = pm->w*u->ipos.y + u->ipos.x;
  int i1 = pm->w*(u->ipos.y + 1) + u->ipos.x;
  int i2 = i0 + 1;

  const vec3 *p[3];
  p[0] = &pm->vcoords[i0];
  p[1] = &pm->vcoords[i1];
  p[2] = &pm->vcoords[i2];

  vec3 v[2];
  vec3_set(&v[0], pos->x, 888.0, pos->z);
  vec3_set(&v[1], pos->x, -888.0, pos->z);
  const vec3 *line[2];
  line[0] = &v[0];
  line[1] = &v[1];

  /* get the normal (core->u) from patchmap planes */
  vec4 *plane = &pm->cells[c_index].planes[0];
  vec3_frm_vec4(&u->u, plane);
  gmth_find_x_point(&u->pos, &u->u, plane->D, line);

  if(!gmth_is_in_polygon(&u->pos, p, 3)) {
    /* get the normal (core->u) from patchmap planes */
    plane = &pm->cells[c_index].planes[1];
    vec3_frm_vec4(&u->u, plane);
    gmth_find_x_point(&u->pos, &u->u, plane->D, line);
  }
}

/* set the unit's direction */
void
unit_set_dir( unit *u, const vec3 *forward )
{
  /* find the unit's direction */
  obj_find_axis(&u->r, &u->u, forward);
  obj_find_axis(&u->f, &u->r, &u->u);
}

/* set unit skin set */
void
unit_set_skin( unit *u, const int skin_index, const int skin_stride, const hashtable *texdb )
{
  ms3d *model = u->model;

  int i;
  for(i = 0; i < model->n_mshs; i++) {
    mesh *msh = u->mshs[i];
    ms3dmesh *o_msh = model->o_mshs[i];

    /* get the material and texture */
    int index = skin_index*skin_stride + (int)o_msh->material_index; 

    ms3dskin *skin;
    skin = &model->o_skins[index];
    msh->tex_diffuse = hash_get(texdb, skin->texture);
    skin = &model->o_skins[index + 1];
    msh->tex_normalmap = hash_get(texdb, skin->texture);
    skin = &model->o_skins[index + 2];
    msh->tex_specular = hash_get(texdb, skin->texture);

    tex_set_flags(*msh->tex_diffuse, TEX_USE_MIPMAPS|TEX_TO_BORDER);
    tex_set_flags(*msh->tex_normalmap, TEX_USE_LINEAR|TEX_TO_BORDER);
    tex_set_flags(*msh->tex_specular, TEX_USE_MIPMAPS|TEX_TO_BORDER);
  }
}

/* update unit model vertices */
void
unit_update_vertices( unit *u )
{
//int flag  = 0;
  int i, j;
  ms3d *model = u->model;

  for(i = 0; i < model->n_mshs; i++) {
    mesh *msh = u->mshs[i];
    ms3dmesh *o_msh = model->o_mshs[i];

    for(j = 0; j < o_msh->n_vertices; j++) {
      int bone_id = (int)o_msh->o_jnt_indices[j];
      const vec3 *o_v = &o_msh->o_vcoords[j];
      const vec3 *o_n = &o_msh->o_normals[j];
      const vec3 *o_t = &o_msh->o_tangents[j];
      vec3 *v = &msh->vcoords[j];
      vec3 *n = &msh->normals[j];
      vec3 *t = &msh->tangents[j];

      if(bone_id == -1) {
        vec3_cpy(v, o_v);
        vec3_cpy(n, o_n);
        vec3_cpy(t, o_t);
      }
      else {
        /* mat_joint_finals are joint related(mapped) final matrices */
        const mat4 *mat_joint_final = &u->mat_joint_finals[bone_id];
        mat4_r_mul_t_vec3(v, mat_joint_final, o_v);
        mat4_r_mul_vec3(n, mat_joint_final, o_n);
        mat4_r_mul_vec3(t, mat_joint_final, o_t);
      }

      /* find the min, max of dynamic AABBox, flag == 0 (first time use)
      if(flag == 0) {
        vec3_cpy(&u->aabb_min, v);
        vec3_cpy(&u->aabb_max, v);
        flag = 1;
      }
      vec3_take_min(&u->aabb_min, v);
      vec3_take_max(&u->aabb_max, v);
      */
    }    /* j */
  }      /* i */
}


/* calculate the unit triangle faces */
void
unit_calc_faces( unit *u )
{
  int i, j;

  ms3d *model = u->model;

  for(i = 0; i < model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    for(j = 0; j < msh->n_faces; j++) {
      int i0 = 3*j;
      int i1 = i0 + 1;
      int i2 = i0 + 2;
      const vec3 *v0 = &msh->vcoords[i0];
      const vec3 *v1 = &msh->vcoords[i1];
      const vec3 *v2 = &msh->vcoords[i2];

      /* save the plane equation for this triangle */
      vec4 *plane = &msh->faces[j];
      gmth_plane_set_by_3points(plane, v0, v1, v2);
    }
  }
}

/* calculate HSR for camera */
void
unit_calc_cam_hsr( unit *u, const vec3 *object_space_unit_to_cam, const int flag )
{
  int i, j;
  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    msh->cam_hsr_count = 0;
    for(j = 0; j < msh->n_faces; j++) {
      const vec4 *plane = &msh->faces[j];

      if(gmth_plane_classify_point(plane, object_space_unit_to_cam) == flag) {
        unsigned short i0 = 3*j;
        unsigned short i1 = i0 + 1;
        unsigned short i2 = i0 + 2;
        msh->hsr_cam_v_indices[msh->cam_hsr_count] = i0;
        msh->cam_hsr_count++;      
        msh->hsr_cam_v_indices[msh->cam_hsr_count] = i1;
        msh->cam_hsr_count++;
        msh->hsr_cam_v_indices[msh->cam_hsr_count] = i2;
        msh->cam_hsr_count++;
      }
    }
  }
}

/* calculate HSR for light */
void
unit_calc_lit_hsr( unit *u, const vec3 *object_space_unit_to_lit, const int flag )
{
  int i, j;
  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    msh->lit_hsr_count = 0;
    for(j = 0; j < msh->n_faces; j++) {
      const vec4 *plane = &msh->faces[j];

      if(gmth_plane_classify_point(plane, object_space_unit_to_lit) == flag) {
        unsigned short i0 = 3*j;
        unsigned short i1 = i0 + 1;
        unsigned short i2 = i0 + 2;
        msh->hsr_lit_v_indices[msh->lit_hsr_count] = i0;
        msh->lit_hsr_count++;
        msh->hsr_lit_v_indices[msh->lit_hsr_count] = i1;
        msh->lit_hsr_count++;
        msh->hsr_lit_v_indices[msh->lit_hsr_count] = i2;
        msh->lit_hsr_count++;
      }
    }
  }
}

/* update VBO */
void
unit_update_vbo( const unit *u )
{
  int i;

  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    /* vertices, normals, tangents */
    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_coords);
    void *v_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_normals);
    void *n_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_tangents);
    void *t_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    memcpy(v_data, msh->vcoords, msh->v_data_size);
    memcpy(n_data, msh->normals, msh->v_data_size);
    memcpy(t_data, msh->tangents, msh->v_data_size);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_coords);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_normals);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_tangents);
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }
}

/* update the camera hsr ibo */
void
unit_update_hsr_cam_ibo( const unit *u )
{
  int i;
  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_cam_elements);
    void *hsr_cam_data = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(hsr_cam_data, msh->hsr_cam_v_indices, msh->cam_hsr_count*sizeof(unsigned short));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }
}

/* update the light hsr ibo */
void
unit_update_hsr_lit_ibo( const unit *u )
{
  int i;
  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_lit_elements);
    void *hsr_lit_data = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(hsr_lit_data, msh->hsr_lit_v_indices, msh->lit_hsr_count*sizeof(unsigned short));
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }
}

/* draw the unit model with normalmap */
void
unit_normalmap_draw( const unit *u, const light *lit, const shader *glsl )
{
  int i;
  ms3d *model = u->model;

  glEnableVertexAttribArray(glsl->attri_v_coord);
  glEnableVertexAttribArray(glsl->attri_v_normal);
  glEnableVertexAttribArray(glsl->attri_v_tangent);
  glEnableVertexAttribArray(glsl->attri_v_texcoord);

  for(i = 0; i < model->n_mshs; i++) {
    mesh *msh = u->mshs[i];
    ms3dmesh *o_msh = model->o_mshs[i];

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *msh->tex_diffuse);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *msh->tex_normalmap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, *msh->tex_specular);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lit->tex_shadow);


    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_coords);
    glVertexAttribPointer(glsl->attri_v_coord,   /* attribute */
                          3,                     /* (x, y, z) */
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_normals);
    glVertexAttribPointer(glsl->attri_v_normal,  /* attribute */
                          3,                     /* (x, y, z) */
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

    glBindBuffer(GL_ARRAY_BUFFER, o_msh->vbo_o_texcoords);
    glVertexAttribPointer(glsl->attri_v_texcoord,  /* attribute */
                          2,                       /* (x, y) */
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_tangents);
    glVertexAttribPointer(glsl->attri_v_tangent,  /* attribute */
                          3,                      /* (x, y, z) */
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

    /* push each element to the vertex shader */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_cam_elements);
    glDrawElements(GL_TRIANGLES, msh->cam_hsr_count, GL_UNSIGNED_SHORT, 0);
  }

  glDisableVertexAttribArray(glsl->attri_v_texcoord);
  glDisableVertexAttribArray(glsl->attri_v_tangent);
  glDisableVertexAttribArray(glsl->attri_v_normal);
  glDisableVertexAttribArray(glsl->attri_v_coord);
}

/* draw the unit model with only vertex coordinates */
void
unit_vertex_draw( const unit *u, const shader *glsl )
{
  int i;

  glEnableVertexAttribArray(glsl->attri_v_coord);

  for(i = 0; i < u->model->n_mshs; i++) {
    mesh *msh = u->mshs[i];

    glBindBuffer(GL_ARRAY_BUFFER, msh->vbo_coords);
    glVertexAttribPointer(glsl->attri_v_coord,   /* attribute */
                          3,                     /* (x, y, z) */
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          0);

    /* push each element to the vertex shader */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, msh->ibo_hsr_lit_elements);
    glDrawElements(GL_TRIANGLES, msh->lit_hsr_count, GL_UNSIGNED_SHORT, 0);
  }

  glDisableVertexAttribArray(glsl->attri_v_coord);
}