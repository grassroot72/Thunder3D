/*----- t3d_frustum.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_frustum_h_
#define _t3d_frustum_h_

#include <t3d_type.h>


struct __frustum {
  float half_near_w;
  float half_near_h;
  float half_far_w;
  float half_far_h;

  float max_distsq;
};


/* create a frustum in memory */
void frustum_set( frustum *f,
                  const float fovy,   /* in degree */
                  const float aspect, /* aspect 4:3, 16:9 etc. */
                  const float near,   /* z-near */
                  const float far );  /* z-far */
/* calculate camera frustum */
void frustum_set_clip_planes( vec4 *clip_planes, const mat4 *clip );
/* check if a point is in the frustum */
int frustum_is_point_in( const vec4 *clip_planes,
                         const float x,
                         const float y,
                         const float z );
/* check if a box is in the frustum */
int frustum_is_box_in( const vec4 *clip_planes,
                       const float x,
                       const float y,
                       const float z,
											 const float half_size_x,
                       const float half_size_y,
                       const float half_size_z );
/* check if a sphere is in the frustum */
int frustum_is_sphere_in( const vec4 *clip_planes,
                          const float x,
                          const float y,
                          const float z,
                          const float radius );


#endif  /* _t3d_frustum_h_ */