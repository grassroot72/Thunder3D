/*----- t3d_vec2.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


/* Returns the length of vec2 */
float
vec2_len( const vec2 *i )
{
  return sqrtf(i->x*i->x + i->y*i->y);
}

/* Returns a normalized copy of v */
void
vec2_norm( vec2 *o )
{
  float len = vec2_len(o);
  if(len < EPSILON) return;

  float ilen = 1.0/len;
  o->x *= ilen;
  o->y *= ilen;
}

/* take the minimum */
void
vec2_take_min( vec2 *o, const vec2 *i )
{
  if(i->x < o->x) o->x = i->x;
  if(i->y < o->y) o->y = i->y;
}

/* take the maximum */
void
vec2_take_max( vec2 *o, const vec2 *i )
{
  if(i->x > o->x) o->x = i->x;
  if(i->y > o->y) o->y = i->y;
}