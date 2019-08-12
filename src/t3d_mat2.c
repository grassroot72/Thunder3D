/*----- t3d_mat2.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>


/* mat2 ---------------------------------------------------------------------+
   0 2           m00 m10
   1 3           m01 m11
 +---------------------------------------------------------------------------*/

/* set the matrix by 2 column vec2 vectors */
void
mat2_v2set( mat2 *o, const vec2 *i0, const vec2 *i1 )
{
  vec2_cpy(&o->c0, i0);
  vec2_cpy(&o->c1, i1);
}

/* inverse a mat2 matrix */
void
mat2_inv( mat2 *o, const mat2 *i )
{
  float det = i->m00*i->m11 - i->m01*i->m10;
  /* cannot inverse, make it identity matrix */
  if(fabs(det) <= EPSILON) {
    mat2_identity(o);
    return;
  }

  float idet = 1.0/det;

  o->m00 =  idet*i->m11;
  o->m01 = -idet*i->m01;
  o->m10 = -idet*i->m10;
  o->m11 =  idet*i->m00;
}