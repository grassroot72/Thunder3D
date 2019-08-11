/*----- t3d_vec3.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


/* calculate the length (magnitude) of vec3 */
float
vec3_len( const vec3 *i )
{
  return sqrtf(i->x*i->x + i->y*i->y + i->z*i->z);
}

/* normalize a 3d vector */
void
vec3_norm( vec3 *o )
{
  float len = vec3_len(o);
  if(len < EPSILON) return;

  float ilen = 1.0/len;
  o->x *= ilen;
  o->y *= ilen;
  o->z *= ilen;
}

/* take the minimum */
void
vec3_take_min( vec3 *o, const vec3 *i )
{
  if(i->x < o->x) o->x = i->x;
  if(i->y < o->y) o->y = i->y;
  if(i->z < o->z) o->z = i->z;
}

/* take the maximum */
void
vec3_take_max( vec3 *o, const vec3 *i )
{
  if(i->x > o->x) o->x = i->x;
  if(i->y > o->y) o->y = i->y;
  if(i->z > o->z) o->z = i->z;
}