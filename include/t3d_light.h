/*----- t3d_light.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_light_h_
#define _t3d_light_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


struct __light {
  int shadow_size;
  GLuint tex_shadow;    /* shadow texture id */
  GLuint fbo_shadow;    /* light specific - FBO shadowmap */

  float move_speed;     /* movement speed */
  float rot_speed;      /* rotation speed */
  float strafe_speed;   /* strafing speed */
  float time;           /* time pass, in micro-seconds */

  vec3 pos;             /* light's position */
  vec3 lookat;          /* light's lookat position */

  vec3 f;               /* light's forward direction */
  vec3 u;               /* light's up direction */
  vec3 r;               /* light's right direction */
  vec3 u_ref;           /* light's up reference (0.0, 1.0, 0.0) */

  vec4 clips[6];        /* frustum clip planes */

  mat4 mat_view;        /* view matrix */
  mat4 mat_proj;        /* projection (ortho) matrix */
  mat4 mat_ortho;       /* ortho matrix, can be scaled according to camera */
  mat4 mat_vp;          /* proj*view matrix */
  mat4 mat_bias;        /* bias matrix for shadowmap */
  mat4 mat_bias_vp;     /* bias*proj*view matrix */
};


/* create light struct in memory */
light *lit_new( const int shadow_size, const vec3 *pos, const vec3 *lookat );
/* destroy light struct */
void lit_del( light *lit );
/* set light's position and lookat */
void lit_set_view( light *lit );


#endif  /* _t3d_light_h_ */