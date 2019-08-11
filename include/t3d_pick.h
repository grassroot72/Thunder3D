/*----- t3d_pick.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_pick_h_
#define _t3d_pick_h_

#include <t3d_type.h>


enum {
  PICK_NULL = 0,
  PICK_PICKING = 1,
  PICK_SINGLE = 2,
  PICK_MULTI = 3,
  PICK_LEFT_PICKED = 4,
  PICK_RIGHT_PICKED = 5
};


/* pickbox ------------------------------------------------------------------+
   v0--+      +---v0
   | / |  or  | / |
	 |/  |      |/  |
	 +---v1     v1--+
 +---------------------------------------------------------------------------*/
struct __pickbox {
  vec2 v0;
  vec2 v1;

  int status;                /* is picking is progress? picked? */
  int type;                  /* single pick or multi pick */

  GLuint vbo_coord;          /* VBO - font vertex coordinate */
  mat4 mat_proj;             /* projection (otho) matrix */
};


/* create a pickbox in memory */
pickbox *pick_new( const int screen_w, const int screen_h );
/* delelte pickbox from memory */
void pick_del( pickbox *pb );
/* calculate the ray direction vector */
void pick_find_ray_end( vec3 *ray_end,           /* vertex coordinate, in world space */
                        const float mouse_x,     /* in pixels, from bottom-left corner window */
                        const float mouse_y,     /* in pixels, from bottom-left corner window */
                        const int screen_w,      /* in pixels, window width */
                        const int screen_h,      /* in pixels, window hwight */
                        const mat4 *mat_vp );    /* proj*view matrix */
/* check if ray intersect with OBB and return the distance between ray_start and the intersection */
int pick_is_ray_x_obb( float *x_dist,           /* distance between ray_start and the intersection with the OBB */
                       const vec3 *ray_start,   /* ray start position, in world space */
                       const vec3 *ray_dir,     /* ray direction (NOT target position!), in world space */
                       const vec3 *aabb_min,    /* minimum X,Y,Z coords */
                       const vec3 *aabb_max,    /* maximum X,Y,Z coords */
                       const mat4 *mat_model ); /* transformation applied to the mesh (aabb) */
/* check picked status */
void pick_check_left_picked( pickbox *pb, const list *units, const camera *cam );
/* check right picked */
void pick_check_right_picked( pickbox *pb, const patchmap *pm, const list *units, const camera *cam );
/* draw the pickbox */
void pick_draw_box2d( const pickbox *pb,
                      const vec2 *v0, const vec2 *v1,
                      const vec4 *color,
                      shader *glsl );


#endif   /* _t3d_pick_h_ */
