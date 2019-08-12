/*----- t3d_scene.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_geomath.h>
#include <t3d_slist.h>
#include <t3d_frustum.h>
#include <t3d_object.h>
#include <t3d_control.h>
#include <t3d_pick.h>
#include <t3d_camera.h>
#include <t3d_heightmap.h>
#include <t3d_geopatch.h>
#include <t3d_patchmap.h>
#include <t3d_ms3d.h>
#include <t3d_unit.h>
#include <t3d_light.h>
#include <t3d_sys.h>
#include <t3d_decal.h>
#include <t3d_aamesh.h>
#include <t3d_astar.h>
#include <t3d_scene.h>


/* create a scene in memory */
scene *
scn_new( t3dsys *sys, camera *cam, light *lit, control *ctrl, pickbox *pickb )
{
  scene *scn = (scene *)malloc(sizeof(scene));

  scn->sys = sys;
  scn->cam = cam;
  scn->lit = lit;
  scn->ctrl = ctrl;
  scn->pickb = pickb;

  scn->geopatch_count = 0;
  scn->unit_count = 0;

  scn->batch_count = 3;

  return scn;
}

/* delete scene from memory */
void
scn_del( scene *scn )
{
  if(!scn) return;

  free(scn);
}

/* update camera and light */
void
scn_update_cam_lit( void *arg_scn )
{
  scene *scn = (scene *)arg_scn;

  camera *cam = scn->cam;
  light *lit = scn->lit;
  control *ctrl = scn->ctrl;
  pickbox *pickb = scn->pickb;

  /* process key events */
  ctrl_process_key(ctrl, cam, lit);
  ctrl_process_mouse(ctrl, cam, lit, pickb);

  /* camera - get view matrix, update camera, calculate the frustum */
  cam_set_view(cam);
  mat4_mul(&cam->mat_vp, &cam->mat_proj, &cam->mat_view);
  mat4_inv_euclidean(&cam->mat_view_inv, &cam->mat_view);
  frustum_set_clip_planes(cam->clips, &cam->mat_vp);

  /* light - get view matrix, update light, calculate the frustum */
  lit_set_view(lit);
  mat4_scale(&lit->mat_ortho, &lit->mat_proj, cam->max_y/cam->pos.y);
  mat4_mul(&lit->mat_vp, &lit->mat_ortho, &lit->mat_view);
  mat4_mul(&lit->mat_bias_vp, &lit->mat_bias, &lit->mat_vp);
  frustum_set_clip_planes(lit->clips, &lit->mat_vp);

  scn->batch_count++;
}

/* update geopatch visibility */
void
scn_update_geopatch( void *arg_scn )
{
  scene *scn = (scene *)arg_scn;

  t3dsys *sys = scn->sys;
  camera *cam = scn->cam;
  light *lit = scn->lit;

  geopatch *pch = sys->pchmap->pchs[scn->geopatch_count];

  vec3 dist;
  vec3_sub(&dist, &cam->pos, &pch->center);
  if(vec3_lensq(&dist) < cam->v_frustum->max_distsq) {
    /* visible to camera? */
    if(frustum_is_box_in(cam->clips,
                         pch->center.x,
                         pch->center.y,
                         pch->center.z,
                         pch->half_x_len,
                         pch->half_y_len,
                         pch->half_z_len)) {
      /* visible to light? */
      if(frustum_is_box_in(lit->clips,
                           pch->center.x,
                           pch->center.y,
                           pch->center.z,
                           pch->half_x_len,
                           pch->half_y_len,
                           pch->half_z_len))
        pch->visible = 1;
      else
        pch->visible = 0;
    }
    else
      pch->visible = 0;
  }
  else
    pch->visible = 0;


  scn->geopatch_count++;
  if(scn->geopatch_count == sys->pchmap->n_total_pchs) {
    scn->geopatch_count = 0;
    scn->batch_count++;
  }
}

/* update units */
void
scn_update_unit( void *arg_scn )
{
  scene *scn = (scene *)arg_scn;

  t3dsys *sys = scn->sys;
  patchmap *pchmap = sys->pchmap;
  camera *cam = scn->cam;
  light *lit = scn->lit;

  /* update units */
  listnode *node = list_node_at(sys->units, scn->unit_count);
  unit *u = (unit *)node->data;

  /* quick check if unit is in the frustum */
  vec3 unit_to_cam_ws;
  vec3_sub(&unit_to_cam_ws, &cam->pos, &u->pos);
  if(vec3_lensq(&unit_to_cam_ws) < cam->v_frustum->max_distsq) {
    mat4_set_model(&u->mat_model, &u->pos, &u->f, &u->u, &u->r);
    mat3_frm_mat4(&u->mat_m3x3, &u->mat_model);

    mat4_t_vec3(&u->center_ws, &u->mat_model, &u->center);

    /* visible to camera? */
    if(frustum_is_box_in(cam->clips,
                         u->center_ws.x,
                         u->center_ws.y,
                         u->center_ws.z,
                         u->half_x_len,
                         u->half_y_len,
                         u->half_z_len)) {
      /* visible to light? */
      if(frustum_is_box_in(lit->clips,
                           u->center_ws.x,
                           u->center_ws.y,
                           u->center_ws.z,
                           u->half_x_len,
                           u->half_y_len,
                           u->half_z_len)) {
        u->visible = 1;

        /* decal aamesh */
        if(u->picked) {
          aamesh_tex_offset(u->aamsh, u->model->o_aamsh, u->pos.x, u->pos.z, sys->pchmap->step);
          aamesh_get_mesh(u->aamsh, u->model->o_aamsh, u->ipos.x, u->ipos.y, sys->pchmap);
        }

        /* update animation, model vertices */
        ms3d_calc_anim_time(u->ani, u->model, sys->time_passed);
        ms3d_animate(u->mat_joint_finals, u->ani, u->model);
        unit_update_vertices(u);

        /* find the object space view vector first */
        vec3 unit_to_lit_ws;
        vec3 unit_to_cam_os;
        vec3 unit_to_lit_os;
        mat4 mat_model_transp;

        vec3_sub(&unit_to_lit_ws, &lit->pos, &u->pos);
        mat4_transpose(&mat_model_transp, &u->mat_model);
        mat4_r_mul_t_vec3(&unit_to_cam_os, &mat_model_transp, &unit_to_cam_ws);
        mat4_r_mul_t_vec3(&unit_to_lit_os, &mat_model_transp, &unit_to_lit_ws);

        /* update units' HSR visibility */
        /* prepare the faces (planes) for HSR */
        unit_calc_faces(u);
        /* update visibility HSR (hidden surface removal) to light */
        unit_calc_lit_hsr(u, &unit_to_lit_os, PLANE_BACK);
        /* update visibility HSR (hidden surface removal) to camera */
        unit_calc_cam_hsr(u, &unit_to_cam_os, PLANE_FRONT);
      }
      else
        u->visible = 0;
    }
    else
      u->visible = 0;
  }
  else
    u->visible = 0;

  /* trigger to calulate the A* path */
  if(u->chk_path) {
    if(u->path) free(u->path);

    /*u->n_steps = astar_search(&u->path, &u->ipos, &u->itarget, pchmap);*/
    u->n_steps = 1;  /* 1 (TRUE) */
    obj_find_f_axis(&u->target_f, &u->pos, &u->last_waypoint);
    /* reset to 0 (FALSE) after finding the path */
    u->chk_path = 0;
    ms3d_set_anim(u->ani, u->model, 'A');
  }

  /* let's finish walking the path */
  if(u->n_steps) {
    /* move the unit */
    float dist = u->move_speed*sys->time_passed;
    obj_move(&u->pos, &u->target, &u->f, dist, OBJ_ON_GROUND);
    /* set unit's postion */
    unit_set_pos(u, &u->pos, pchmap);
    /* find unit's forward direction */
    obj_find_f_axis(&u->f, &u->pos, &u->last_waypoint);
    /* set unit's up and right directions */
    unit_set_dir(u, &u->f);

    if(ivec2_eq(&u->ipos, &u->itarget)) {
      free(u->path);
      u->n_steps = 0;

      /* set unit's direction */
      unit_set_pos(u, &u->pos, sys->pchmap);
      unit_set_dir(u, &u->target_f);
      ms3d_set_anim(u->ani, u->model, 'N');
    }
  }

  /* theading count */
  scn->unit_count++;
  if(scn->unit_count == sys->units->size) {
    scn->unit_count = 0;
    scn->batch_count++;
  }
}

/* update vbo, ibos in the scene */
void
scn_update_vbo_ibo( const t3dsys *sys )
{
  int i;
  for(i = 0; i < sys->units->size; i++) {
    listnode *node = list_node_at(sys->units, i);
    unit *u = (unit *)node->data;

    if(u->visible) {
      /* must update vbo, ibo in main thread, so do it in light pass */
      /* update vbo, ibo */
      unit_update_vbo(u);
      unit_update_hsr_lit_ibo(u);
      unit_update_hsr_cam_ibo(u);
    }
  }
}

/* prepare light pass shader */
static void
scn_map_to_light_shader( shader *glsl,
                         const float *lit_mat_vp,
                         const float *unit_mat_model )
{
  glUniformMatrix4fv(glsl->uniform_lvp, 1, GL_FALSE, lit_mat_vp);
  glUniformMatrix4fv(glsl->uniform_m, 1, GL_FALSE, unit_mat_model);
}

/* the light pass to generate shadowmap */
void
scn_light_pass( const t3dsys *sys, const light *lit, shader *glsl )
{
  /* draw the shadowmap to framebuffer */
  glBindFramebuffer(GL_FRAMEBUFFER, lit->fbo_shadow);

  glViewport(0, 0, lit->shadow_size, lit->shadow_size);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glClear(GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_FRONT);

  glUseProgram(glsl->prog_light_pass);
  shd_set(glsl, glsl->prog_light_pass);
/*
  patchmap *pchmap = sys->pchmap;
  scn_map_to_light_shader(glsl, lit->mat_vp.m, pchmap->mat_model.m);
  pchmap_vertex_draw(pchmap, glsl);
*/

  int i;
  for(i = 0; i < sys->units->size; i++) {
    listnode *node = list_node_at(sys->units, i);
    unit *u = (unit *)node->data;

   if(u->visible) {
      /* vs */
      scn_map_to_light_shader(glsl, lit->mat_vp.m, u->mat_model.m);
      unit_vertex_draw(u, glsl);
    }
  }
}

/* prepare normalmap pass shader */
static void
scn_map_to_normalmap_shader( shader *glsl,
                             const float *cam_mat_vp,
                             const float *obj_mat_model,
                             const float *obj_m3x3_inv_transp,
                             const float *lit_mat_bias_vp,
                             const float *cam_mat_view_inv,
                             const vec4 *light_pos )
{
  /* vs */
  glUniformMatrix4fv(glsl->uniform_vp, 1, GL_FALSE, cam_mat_vp);
  glUniformMatrix4fv(glsl->uniform_m, 1, GL_FALSE, obj_mat_model);
  /*---------------------------------------------------------------------------+
    If you don't have non-uniform scaling as part of your model matrix,
    the upper 3x3 of the model matrix is just as good as the "normal matrix"
    (which is the transpose of the inverse of the model matrix) for 
    transforming normals
   +--------------------------------------------------------------------------*/
  glUniformMatrix3fv(glsl->uniform_m3x3_inv_transp, 1, GL_FALSE, obj_m3x3_inv_transp);
  glUniformMatrix4fv(glsl->uniform_bias_lvp, 1, GL_FALSE, lit_mat_bias_vp);
  /* fs */
  glUniformMatrix4fv(glsl->uniform_v_inv, 1, GL_FALSE, cam_mat_view_inv);
  glUniform1i(glsl->uniform_diffmap, /*GL_TEXTURE*/0);
  glUniform1i(glsl->uniform_normmap, /*GL_TEXTURE*/1);
  glUniform1i(glsl->uniform_specmap, /*GL_TEXTURE*/2);
  glUniform1i(glsl->uniform_shadmap, /*GL_TEXTURE*/3);
  glUniform4f(glsl->uniform_light_coord, light_pos->x, light_pos->y, light_pos->z, light_pos->w);
}

/* the camera pass */
void
scn_camera_pass( const t3dsys *sys, const camera *cam, const light *lit, shader *glsl )
{
  int i;
  /* light position */
  vec4 light_pos;
  vec4_frm_v3w(&light_pos, &lit->pos, 0.0);

  /* bind to 0 to draw normally */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClearColor(0.2, 0.2, 0.2, 0.2);
  glViewport(0, 0, cam->screen_w, cam->screen_h);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);

  /* draw normalmap patchmap */
  glUseProgram(glsl->prog_normal_pass);
  shd_set(glsl, glsl->prog_normal_pass);

  patchmap *pchmap = sys->pchmap;
  scn_map_to_normalmap_shader(glsl,
                              cam->mat_vp.m,
                              pchmap->mat_model.m, pchmap->mat_m3x3.m,
                              lit->mat_bias_vp.m,
                              cam->mat_view_inv.m,
                              &light_pos);
  pchmap_normalmap_draw(pchmap, lit, glsl);


  /* draw aamesh - transparent */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(glsl->prog_aamesh);
  shd_set(glsl, glsl->prog_aamesh);

  for(i = 0; i < sys->units->size; i++) {
    listnode *node = list_node_at(sys->units, i);
    unit *u = (unit *)node->data;

    if(u->visible && u->picked) {
      aamesh_update_vbo(u->aamsh);
      aamesh_draw(u->aamsh, u->model->o_aamsh, cam, &sys->colors[u->color], glsl);
    }
  }
  glDisable(GL_BLEND);


  /* draw normalmap units */
  glUseProgram(glsl->prog_normal_pass);
  shd_set(glsl, glsl->prog_normal_pass);

  for(i = 0; i < sys->units->size; i++) {
    listnode *node = list_node_at(sys->units, i);
    unit *u = (unit *)node->data;

    if(u->visible) {
      scn_map_to_normalmap_shader(glsl,
                                  cam->mat_vp.m,
                                  u->mat_model.m, u->mat_m3x3.m,
                                  lit->mat_bias_vp.m,
                                  cam->mat_view_inv.m,
                                  &light_pos);
      unit_normalmap_draw(u, lit, glsl);
    }
  }
}