/*----- t3d_camera.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>
#include <t3d_object.h>
#include <t3d_frustum.h>
#include <t3d_patchmap.h>
#include <t3d_camera.h>


/* create camera struct in memory */
camera *
cam_new( const int mode, const vec3 *pos, const vec3 *lookat,
         const int screen_w, const int screen_h, const patchmap *pm )
{
  camera *cam = (camera *)malloc(sizeof(camera));

  cam->screen_w = screen_w;
  cam->screen_h = screen_h;
  float aspect = (float)screen_w/(float)screen_h;

  /* set camera move speed */
  cam->rot_speed = 0.08;
  cam->strafe_speed = 0.08;
  cam->move_speed = 0.28;
  cam->zoom_speed = 0.80;
  cam->time = 0.0;

  cam->max_y = pos->y;
  cam->min_y = 258.0;

  cam->max_x = pm->step*pm->w_cells - 588.0;
  cam->min_x = 8.0;

  cam->max_z = pm->step*pm->h_cells - 498.0;
  cam->min_z = 498.0;

  vec3_cpy(&cam->pos, pos);
  vec3_cpy(&cam->lookat, lookat);
  vec3_set(&cam->u_ref, 0.0, 1.0, 0.0);

  cam->mode = mode;
  cam->v_frustum = (frustum *)malloc(sizeof(frustum));

  mat4_set_proj(&cam->mat_proj, 30.0, aspect, 1.8, 1580.0);
  frustum_set(cam->v_frustum, 30.0, aspect, 1.8, 1580.0);

  return cam;
}

/* destroy camera struct */
void
cam_del( camera *cam )
{
  if(!cam) return;

  free(cam->v_frustum);
  free(cam);
}

/* set camera's view */
void
cam_set_view( camera *cam )
{
  /* for view matrix, need to negate the forward vector, so we use
     camera's (forward = pos - lookat) */
  obj_find_f_axis(&cam->f, &cam->lookat, &cam->pos);
  obj_find_axis(&cam->r, &cam->u_ref, &cam->f);
  obj_find_axis(&cam->u, &cam->f, &cam->r);
  mat4_set_view(&cam->mat_view, &cam->pos, &cam->f, &cam->u, &cam->r);
}