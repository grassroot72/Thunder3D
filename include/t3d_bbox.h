/*----- t3d_bbox.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_bbox_h_
#define _t3d_bbox_h_

#include <t3d_type.h>


/* calculate the size of an AABBox */
void aabb_calc_size( vec3 *center,
                     float *half_x_len,
                     float *half_y_len,
                     float *half_z_len,
                     const vec3 *aabb_min,
                     const vec3 *aabb_max );


#endif  /* _t3d_bbox_h_ */