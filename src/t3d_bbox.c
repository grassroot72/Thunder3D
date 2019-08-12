/*----- t3d_bbox.c -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <t3d_math.h>
#include <t3d_bbox.h>


/* calculate the size of an AABBox */
void
aabb_calc_size( vec3 *center,
                float *half_x_len,
                float *half_y_len,
                float *half_z_len,
                const vec3 *aabb_min,
                const vec3 *aabb_max )
{
  vec3_set(center, (aabb_min->x + aabb_max->x)*0.5,
                   (aabb_min->y + aabb_max->y)*0.5,
                   (aabb_min->z + aabb_max->z)*0.5);
  *half_x_len = (aabb_max->x - aabb_min->x)*0.5;
  *half_y_len = (aabb_max->y - aabb_min->y)*0.5;
  *half_z_len = (aabb_max->z - aabb_min->z)*0.5;
}