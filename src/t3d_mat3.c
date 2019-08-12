/*----- t3d_mat3.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>


/* mat3 ---------------------------------------------------------------------+
   0 3 6         m00 m10 m20
   1 4 7         m01 m11 m21
   2 5 8         m02 m12 m22
 +---------------------------------------------------------------------------*/

/* note: entered in rows, but stored in columns */
void
mat3_set( mat3 *o,
          const float x0, const float y0, const float z0,
          const float x1, const float y1, const float z1,
          const float x2, const float y2, const float z2 )
{
	o->m00 = x0;        /* column 0 */
	o->m01 = y0;        /* column 0 */
	o->m02 = z0;        /* column 0 */

	o->m10 = x1;        /* column 1 */
	o->m11 = y1;        /* column 1 */
	o->m12 = z1;        /* column 1 */

	o->m20 = x2;        /* column 2 */
	o->m21 = y2;        /* column 2 */
	o->m22 = z2;        /* column 2 */
}

/* set the matrix by 3 column vec3 vectors */
void
mat3_v3set( mat3 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2 )
{
  vec3_cpy(&o->c0, i0);
  vec3_cpy(&o->c1, i1);
  vec3_cpy(&o->c2, i2);
}

/* copy a matrix from another matrix */
void
mat3_cpy( mat3 *o, const mat3 *i )
{
  /* memcpy might not be faster here? */
  o->m[0] = i->m[0];
  o->m[1] = i->m[1];
  o->m[2] = i->m[2];
  o->m[3] = i->m[3];
  o->m[4] = i->m[4];
  o->m[5] = i->m[5];
  o->m[6] = i->m[6];
  o->m[7] = i->m[7];
  o->m[8] = i->m[8];
}

/* copy mat3 from upper left part of mat4 */
void
mat3_frm_mat4( mat3 *o, const mat4 *i )
{
  o->m00 = i->m00;
  o->m01 = i->m01;
  o->m02 = i->m02;

  o->m10 = i->m10;
  o->m11 = i->m11;
  o->m12 = i->m12;

  o->m20 = i->m20;
  o->m21 = i->m21;
  o->m22 = i->m22;
}

/* get a 9-element array flipped on the main diagonal */
void
mat3_transpose( mat3 *o, const mat3 *i )
{
  o->m00 = i->m00;
  o->m01 = i->m10;
  o->m02 = i->m20;

  o->m10 = i->m01;
  o->m11 = i->m11;
  o->m12 = i->m21;

  o->m20 = i->m02;
  o->m21 = i->m12;
  o->m22 = i->m22;
}

/* returns a scalar value with the determinant for mat3 */
float
mat3_determinant( const mat3 *i )
{
  float det;

  det = i->m00*i->m11*i->m22 +
        i->m01*i->m12*i->m20 +
        i->m02*i->m10*i->m21 -
        i->m00*i->m12*i->m21 -
        i->m01*i->m10*i->m22 -
        i->m02*i->m11*i->m20;

  return det;
}

/* inverse a mat3 matrix - general mathematical way */
void
mat3_inv_math( mat3 *o, const mat3 *i )
{
  float det;

  det = mat3_determinant(i);
  /* if cannot inverse, make it identity matrix */
  if(fabs(det) <= EPSILON) {
    mat3_identity(o);
    return;
  }

  float idet = 1.0/det;
  o->m00 = (i->m11*i->m22 - i->m12*i->m21)*idet;
  o->m01 = (i->m02*i->m21 - i->m01*i->m22)*idet;
  o->m02 = (i->m01*i->m12 - i->m02*i->m11)*idet;

  o->m10 = (i->m12*i->m20 - i->m10*i->m22)*idet;
  o->m11 = (i->m00*i->m22 - i->m02*i->m20)*idet;
  o->m12 = (i->m02*i->m10 - i->m00*i->m12)*idet;

  o->m20 = (i->m10*i->m21 - i->m11*i->m20)*idet;
  o->m21 = (i->m01*i->m20 - i->m00*i->m21)*idet;
  o->m22 = (i->m00*i->m11 - i->m01*i->m10)*idet;
}

/* inverse a mat3 matrix - optimized a little bit */
void
mat3_inv( mat3 *o, const mat3 *i )
{
  float det;
  float c00, c01, c02;   /* column 0 */
  float c10, c11, c12;   /* column 1 */
  float c20, c21, c22;   /* column 2 */

  c00 = i->m11*i->m22 - i->m21*i->m12;
  c01 = i->m21*i->m02 - i->m01*i->m22;
  c02 = i->m01*i->m12 - i->m11*i->m02;

  /* check determinant if it is 0 */
  det = i->m00*c00 + i->m10*c01 + i->m20*c02;
  /* if cannot inverse, make it identity matrix */
  if(fabs(det) <= EPSILON) {
    mat3_identity(o);
    return;
  }

  c10 = i->m20*i->m12 - i->m10*i->m22;
  c11 = i->m00*i->m22 - i->m20*i->m02;
  c12 = i->m10*i->m02 - i->m00*i->m12;

  c20 = i->m10*i->m21 - i->m20*i->m11;
  c21 = i->m20*i->m01 - i->m00*i->m21;
  c22 = i->m00*i->m11 - i->m10*i->m01;

  /* divide by the determinant */
  float idet = 1.0/det;
  o->m00 = idet*c00;
  o->m01 = idet*c01;
  o->m02 = idet*c02;
  o->m10 = idet*c10;
  o->m11 = idet*c11;
  o->m12 = idet*c12;
  o->m20 = idet*c20;
  o->m21 = idet*c21;
  o->m22 = idet*c22;
}