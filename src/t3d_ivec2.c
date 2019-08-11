/*----- t3d_ivec2.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


/* check if 2 ivec2s are equal */
int
ivec2_eq( const ivec2 *i1, const ivec2 *i2 )
{
  return (i1->x == i2->x && i1->y == i2->y);
}