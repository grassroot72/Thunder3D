/*----- t3d_bezier.c --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>
#include <t3d_bezier.h>


/* get a list of bezier points */
vec2 *
bezier_get_2d_points( const vec2 *points, const int n_points, const float t )
{
  int data_size = sizeof(vec2)*n_points;
  vec2 *bz_points = (vec2 *)malloc(data_size);
  memcpy(bz_points, points, data_size);

  int i = n_points - 1;
  int k;
  while(i > 0) {
    for(k = 0; k < i; k++) {
      vec2 tmp;
      vec2_cpy(&tmp, &bz_points[k]);
      vec2_lerp(&bz_points[k], &tmp, &bz_points[k + 1], t);
    }
    i--;
  }

  return bz_points;
}