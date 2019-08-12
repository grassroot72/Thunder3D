/*----- t3d_vec4.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


/* normalize a 4d vector */
void
vec4_norm( vec4 *o )
{
  float len = sqrtf(o->x*o->x + o->y*o->y + o->z*o->z);
  if(len < EPSILON) return;

  float ilen = 1.0/len;
  o->x *= ilen;
  o->y *= ilen;
  o->z *= ilen;
  o->w *= ilen;
}