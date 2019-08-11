/*----- t3d_pick.c -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_geomath.h>
#include <t3d_shader.h>
#include <t3d_slist.h>
#include <t3d_object.h>
#include <t3d_camera.h>
#include <t3d_geopatch.h>
#include <t3d_patchmap.h>
#include <t3d_ms3d.h>
#include <t3d_unit.h>
#include <t3d_astar.h>
#include <t3d_pick.h>


/* store vertices, normals and/or elements in graphic card buffers */
static void
pick_upload( const pickbox *pb )
{
  glBindBuffer(GL_ARRAY_BUFFER, pb->vbo_coord);
  /* 10 vertices (vec2) of pickbox, check pick_draw_box2d */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*10, NULL, GL_DYNAMIC_DRAW);
}

/* create a pickbox in memory */
pickbox *
pick_new( const int screen_w, const int screen_h )
{
  pickbox *pb = (pickbox *)malloc(sizeof(pickbox));

  pb->status = PICK_NULL;
  pb->type = PICK_NULL;

  /* create the vertex buffer object for box (selection box) */
  glGenBuffers(1, &pb->vbo_coord);
  pick_upload(pb);

  mat4_set_ortho(&pb->mat_proj, 0.0, (float)screen_w, 0.0, (float)screen_h, -10.0, 10.0);

  return pb;
}

/* delelte pickbox from memory */
void
pick_del( pickbox *pb )
{
  if(!pb)  return;

  glDeleteBuffers(1, &pb->vbo_coord);
  free(pb);
}

/* calculate the ray end */
void
pick_find_ray_end( vec3 *ray_end,          /* vertex coordinate, in world space */
                   const float mouse_x,    /* in pixels, NDC coordinates */
                   const float mouse_y,    /* in pixels, NDC coordinates */
                   const int screen_w,     /* in pixels, window width */
                   const int screen_h,     /* in pixels, window hwight */
                   const mat4 *mat_vp )     /* proj*view matrix */
{
  vec4 ray_end_ndc;
  vec4_set(&ray_end_ndc, (mouse_x/(float)screen_w - 0.5)*2.0,
                         (mouse_y/(float)screen_h - 0.5)*2.0,
                         0.0,
                         1.0);

  mat4 mat_vp_inv;
  /*-------------------------------------------------------------------------+
     The projection matrix goes from camera space to NDC,
     so inverse(projection matrix) goes from NDC to camera space.
     The view matrix goes from world space to camera space,
     so inverse(view matrix) goes from camera space to world space.
     then, inverse proj*view (mat_vp) goes directly from NDC to world space.
   *-------------------------------------------------------------------------*/
  mat4_inv(&mat_vp_inv, mat_vp);

  vec4 ray_end_ws;   /* ray end in world space */
  mat4_mul_vec4(&ray_end_ws, &mat_vp_inv, &ray_end_ndc);
  vec4_self_scale(&ray_end_ws, 1.0/ray_end_ws.w);

  vec3_frm_vec4(ray_end, &ray_end_ws);
}

int
pick_is_ray_x_obb( float *x_dist,          /* distance between ray_start and the intersection with the OBB */
                   const vec3 *ray_start,  /* ray start position, in world space */
                   const vec3 *ray_dir,    /* ray direction (NOT target position!), in world space */
                   const vec3 *aabb_min,   /* minimum X,Y,Z coords */
                   const vec3 *aabb_max,   /* maximum X,Y,Z coords */
                   const mat4 *mat_model ) /* transformation applied to the mesh (aabb) */
{
  /* intersection method from Real-Time Rendering and Essential Mathematics for Games */
  float t_min = 0.0;
	float t_max = 100000.0;

  vec3 obb_pos_ws;  /* obb world space position */
  vec3_set(&obb_pos_ws, mat_model->c3.x, mat_model->c3.y, mat_model->c3.z);

  vec3 delta;
  vec3_sub(&delta, &obb_pos_ws, ray_start);


  float e, f, t1, t2;
  /* test intersection with the 2 planes perpendicular to the OBB's X axis */
  vec3 x_axis;
  vec3_set(&x_axis, mat_model->c0.x, mat_model->c0.y, mat_model->c0.z);

  e = vec3_dot(&x_axis, &delta);
  f = vec3_dot(ray_dir, &x_axis);

  /* standard case */
  if(fabs(f) > 0.001) {
    t1 = (e + aabb_min->x)/f;  /* intersection with the "left" plane */
    t2 = (e + aabb_max->x)/f;  /* intersection with the "right" plane */
    /* t1 and t2 now contain distances betwen ray start and ray-plane intersections */

    /* we want t1 to represent the nearest intersection */
    /* so if it's not the case, invert t1 and t2 */
    if(t1 > t2) {
      /* swap t1 and t2 */
      float w = t1;
      t1 = t2;
      t2 = w;    
    }

    /* t_max is the nearest "far" intersection (amongst the X,Y and Z planes pairs) */
    if(t2 < t_max)  t_max = t2;
    /* tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs) */
    if(t1 > t_min)  t_min = t1;

    /* and here's the trick: */
    /* if "far" is closer than "near", then there is NO intersection */
    if(t_min > t_max)  return 0;    /* 0 FALSE */
  }
  /* rare case: the ray is almost parallel to the planes, so they don't have any "intersection" */
  else {
    if(-e + aabb_min->x > 0.0 || -e + aabb_max->x < 0.0)
      return 0;    /* 0 FALSE */
  }


  /* test intersection with the 2 planes perpendicular to the OBB's Y axis */
  vec3 y_axis;
  vec3_set(&y_axis, mat_model->c1.x, mat_model->c1.y, mat_model->c1.z);

  e = vec3_dot(&y_axis, &delta);
  f = vec3_dot(ray_dir, &y_axis);

  if(fabs(f) > 0.001) {
    t1 = (e + aabb_min->y)/f;
    t2 = (e + aabb_max->y)/f;

    if(t1 > t2) {
      /* swap t1 and t2 */
      float w = t1;
      t1 = t2;
      t2 = w;    
    }

    if(t2 < t_max)  t_max = t2;
    if(t1 > t_min)  t_min = t1;
    if(t_min > t_max)  return 0;
  }
  else {
    if(-e + aabb_min->y > 0.0 || -e + aabb_max->y < 0.0)
      return 0;
  }


  /* test intersection with the 2 planes perpendicular to the OBB's Z axis */
  vec3 z_axis;
  vec3_set(&z_axis, mat_model->c2.x, mat_model->c2.y, mat_model->c2.z);

  e = vec3_dot(&z_axis, &delta);
  f = vec3_dot(ray_dir, &z_axis);

  if(fabs(f) > 0.001) {
    t1 = (e + aabb_min->z)/f;
    t2 = (e + aabb_max->z)/f;

    if(t1 > t2) {
      /* swap t1 and t2 */
      float w = t1;
      t1 = t2;
      t2 = w;    
    }

    if(t2 < t_max)  t_max = t2;
    if(t1 > t_min)  t_min = t1;
    if(t_min > t_max)  return 0;
  }
  else {
    if(-e + aabb_min->z > 0.0 || -e + aabb_max->z < 0.0)
      return 0;
  }

  *x_dist = t_min;

  return 1;   /* 1 TRUE */
}

/* check left picked status */
void
pick_check_left_picked( pickbox *pb, const list *units, const camera *cam )
{
  if(pb->status == PICK_LEFT_PICKED) {
    int i;
    /* single picking */
    if(pb->type == PICK_SINGLE) {
      for(i = 0; i < units->size; i++) {
        listnode *node = list_node_at(units, i);
        unit *u = (unit *)node->data;

        vec3 ray_end, ray_dir;
        pick_find_ray_end(&ray_end, 
                          pb->v0.x,
                          pb->v0.y,
                          cam->screen_w,
                          cam->screen_h,
                          &cam->mat_vp);

        vec3_sub(&ray_dir, &ray_end, &cam->pos);
        /* vec3_norm(&ray_dir); */

        float x_dist;
        if(pick_is_ray_x_obb(&x_dist,
                             &cam->pos,
                             &ray_dir,
                             &u->model->aabb_min,
                             &u->model->aabb_max,
                             &u->mat_model))
          u->picked = 1;
        else
          u->picked = 0;
      }
    }

    /* multiple picking */
    if(pb->type == PICK_MULTI) {
      for(i = 0; i < units->size; i++) {
        listnode *node = list_node_at(units, i);
        unit *u = (unit *)node->data;

        vec4 pos_ndc, pos;
        vec4_frm_v3w(&pos, &u->center_ws, 1.0);
        mat4_mul_vec4(&pos_ndc, &cam->mat_vp, &pos);
        vec4_self_scale(&pos_ndc, 1.0/pos_ndc.w);

        float win_x = (1.0 + pos_ndc.x)*cam->screen_w*0.5;
        float win_y = (1.0 + pos_ndc.y)*cam->screen_h*0.5;

        if(win_x > pb->v0.x && win_x < pb->v1.x &&
           win_y > pb->v1.y && win_y < pb->v0.y)
          u->picked = 1;
        else
          u->picked = 0;
      }
    }

    pb->status = PICK_NULL;
    pb->type = PICK_NULL;
  }
}

/* check right picked */
void
pick_check_right_picked( pickbox *pb, const patchmap *pm, const list *units, const camera *cam )
{
  if(pb->status == PICK_RIGHT_PICKED) {
    vec3 ray_end, ray_dir;
    pick_find_ray_end(&ray_end, 
                      pb->v0.x,
                      cam->screen_h - pb->v0.y,
                      cam->screen_w,
                      cam->screen_h,
                      &cam->mat_vp);
    vec3_sub(&ray_dir, &cam->pos, &ray_end);
    vec3_norm(&ray_dir);
    vec3_self_scale(&ray_dir, 2888.0);
    vec3_sub(&ray_end, &cam->pos, &ray_dir);

    vec3 target;
    int i;
    for(i = 0; i < pm->n_total_pchs; i++) {
      geopatch *pch = pm->pchs[i];
 
      if(pch->visible) {
        /*----- first triangle ----------------------------------------------*/
        vec3 v[3];
        v[0].x = pch->center.x - pch->half_x_len;
        v[0].y = pch->center.y;
        v[0].z = pch->center.z - pch->half_z_len;

        v[1].x = v[0].x;
        v[1].y = pch->center.y;
        v[1].z = pch->center.z + pch->half_z_len;

        v[2].x = pch->center.x + pch->half_x_len;
        v[2].y = pch->center.y;
        v[2].z = v[0].z;

        const vec3 *p[3];
        p[0] = &v[0];
        p[1] = &v[1];
        p[2] = &v[2];

        const vec3 *line[2];
        line[0] = &cam->pos;
        line[1] = &ray_end;

        if(gmth_is_line_x_polygon(&target, p, 3, line)) {
          break;
        }

        /*----- 2nd triangle ------------------------------------------------*/
        v[0].x = v[2].x;
        v[2].z = v[1].z;

        if(gmth_is_line_x_polygon(&target, p, 3, line)) {
          break;
        }
      }
    }

    for(i = 0; i < units->size; i++) {
      listnode *node = list_node_at(units, i);
      unit *u = (unit *)node->data;

      if(u->picked) {
        u->itarget.x = (int)(target.x/pm->step);
        u->itarget.y = (int)(target.z/pm->step);
        vec3_cpy(&u->last_waypoint, &target);
        /* need to calculate the A* path, 1 (TRUE) */
        u->chk_path = 1;
      }
    }

    pb->status = PICK_NULL;
    pb->type = PICK_NULL;
  }
}

/* draw the pickbox */
void
pick_draw_box2d( const pickbox *pb,
                 const vec2 *v0, const vec2 *v1,  /* in ortho projection coordinates */
                 const vec4 *color,
                 shader *glsl )
{
  glUseProgram(glsl->prog_box2d);
  shd_set(glsl, glsl->prog_box2d);

  glUniformMatrix4fv(glsl->uniform_mvp, 1, GL_FALSE, pb->mat_proj.m);
  glUniform4f(glsl->uniform_onecolor, color->r, color->g, color->b, color->a);

  glBindBuffer(GL_ARRAY_BUFFER, pb->vbo_coord);
  glEnableVertexAttribArray(glsl->attri_v_coord);

  void *pb_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

  vec2 *v = (vec2 *)pb_data;
  v[0].x = v0->x;        v[0].y = v0->y;
  v[1].x = v0->x + 2.0;  v[1].y = v0->y + 2.0;
  v[2].x = v1->x;        v[2].y = v0->y;
  v[3].x = v1->x - 2.0;  v[3].y = v0->y + 2.0;
  v[4].x = v1->x;        v[4].y = v1->y;
  v[5].x = v1->x - 2.0;  v[5].y = v1->y - 2.0;
  v[6].x = v0->x;        v[6].y = v1->y;
  v[7].x = v0->x + 2.0;  v[7].y = v1->y - 2.0;
  v[8].x = v0->x;        v[8].y = v0->y;
  v[9].x = v0->x + 2.0;  v[9].y = v0->y + 2.0;

  glUnmapBuffer(GL_ARRAY_BUFFER);

  glVertexAttribPointer(glsl->attri_v_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);

  glDisableVertexAttribArray(glsl->attri_v_coord);
}