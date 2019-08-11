/*----- t3d_camera.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_camera_h_
#define _t3d_camera_h_

#include <t3d_type.h>


enum {
  CAM_FPS_MODE = 1,     /* 1st person camera mode */
  CAM_TPS_MODE = 2,     /* 3rd person camera mode */
  CAM_FLY_MODE = 3      /* flying camera mode */
};

struct __camera {
  int screen_w;         /* screen (viewport) width */
  int screen_h;         /* screen (viewport) height */

  int mode;             /* camera mode, 1st or 3rd person mode */

  float move_speed;     /* movement speed */
  float rot_speed;      /* rotation speed */
  float strafe_speed;   /* strafing speed */
  float zoom_speed;     /* zoom speed */
  float time;           /* time pass, in micro-seconds */

  float max_y;          /* highest vertex's y */
  float min_y;          /* lowest vertex's y */

  float max_x;          /* max x */
  float min_x;          /* min x */

  float max_z;          /* max z */
  float min_z;          /* min z */

  vec3 pos;             /* camera's position */
  vec3 lookat;          /* camera's lookat position */

  vec3 f;               /* camera's forward direction */
  vec3 u;               /* camera's up direction */
  vec3 r;               /* camera's right direction */
  vec3 u_ref;           /* camera's up reference (0.0, 1.0, 0.0) */

  frustum *v_frustum;   /* camera's frustum */
  vec4 clips[6];        /* frustum clip planes */

  mat4 mat_view;        /* view matrix */
  mat4 mat_view_inv;    /* inversed matrix */
  mat4 mat_proj;        /* projection matrix */
  mat4 mat_vp;          /* proj*view matrix */
};


/* create camera struct in memory */
camera *cam_new( const int mode, const vec3 *pos, const vec3 *lookat,
                 const int screen_w, const int screen_h, const patchmap *pm );
/* destroy camera struct */
void cam_del( camera *cam );
/* set camera's view */
void cam_set_view( camera *cam );


#endif  /* _t3d_camera_h_ */