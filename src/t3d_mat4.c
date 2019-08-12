/*----- t3d_mat4.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_math.h>


/* mat4 ---------------------------------------------------------------------+
   0 4  8 12        m00 m10 m20 m30
   1 5  9 13        m01 m11 m21 m31
   2 6 10 14        m02 m12 m22 m32
   3 7 11 15        m03 m13 m23 m33
 +---------------------------------------------------------------------------*/

/* note: entered in rows, but stored in columns */
void
mat4_set( mat4 *o,
          const float x0, const float y0, const float z0, const float w0,
          const float x1, const float y1, const float z1, const float w1,
          const float x2, const float y2, const float z2, const float w2,
          const float x3, const float y3, const float z3, const float w3 )
{
	o->m00 = x0;        /* column 0 */
	o->m01 = y0;        /* column 0 */
	o->m02 = z0;        /* column 0 */
	o->m03 = w0;        /* column 0 */

	o->m10 = x1;        /* column 1 */
	o->m11 = y1;        /* column 1 */
	o->m12 = z1;        /* column 1 */
	o->m13 = w1;        /* column 1 */

	o->m20 = x2;        /* column 2 */
	o->m21 = y2;        /* column 2 */
	o->m22 = z2;        /* column 2 */
	o->m23 = w2;        /* column 2 */

	o->m30 = x3;        /* column 3 */
	o->m31 = y3;        /* column 3 */
	o->m32 = z3;        /* column 3 */
	o->m33 = w3;        /* column 3 */
}

/* set the matrix by 4 column vec4 vectors */
void
mat4_v4set( mat4 *o, const vec4 *i0, const vec4 *i1, const vec4 *i2, const vec4 *i3 )
{
  vec4_cpy(&o->c0, i0);
  vec4_cpy(&o->c1, i1);
  vec4_cpy(&o->c2, i2);
  vec4_cpy(&o->c3, i3);
}

/* set part of the matrix by 4 column vec3 vectors */
void
mat4_v3set_3x4( mat4 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2, const vec3 *i3 )
{
  vec3_cpy(&o->v0, i0);
  vec3_cpy(&o->v1, i1);
  vec3_cpy(&o->v2, i2);
  vec3_cpy(&o->v3, i3);
}

/* set part of the matrix by 3 column vec3 vectors */
void
mat4_v3set_3x3( mat4 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2 )
{
  vec3_cpy(&o->v0, i0);
  vec3_cpy(&o->v1, i1);
  vec3_cpy(&o->v2, i2);
}

/* copy a matrix from another matrix */
void
mat4_cpy( mat4 *o, const mat4 *i )
{
  /* memcpy might not be faster here? */
  o->m[ 0] = i->m[ 0];
  o->m[ 1] = i->m[ 1];
  o->m[ 2] = i->m[ 2];
  o->m[ 3] = i->m[ 3];
  o->m[ 4] = i->m[ 4];
  o->m[ 5] = i->m[ 5];
  o->m[ 6] = i->m[ 6];
  o->m[ 7] = i->m[ 7];
  o->m[ 8] = i->m[ 8];
  o->m[ 9] = i->m[ 9];
  o->m[10] = i->m[10];
  o->m[11] = i->m[11];
  o->m[12] = i->m[12];
  o->m[13] = i->m[13];
  o->m[14] = i->m[14];
  o->m[15] = i->m[15];
}

/* get a row of a matrix, 3 elements */
void
mat4_cpy_v3row( vec3 *o, const mat4 *i, const int pos )
{
  if(pos == 0)
    vec3_set(o, i->m00, i->m10, i->m20);
  else if(pos == 1)
    vec3_set(o, i->m01, i->m11, i->m21);
  else if(pos == 2)
    vec3_set(o, i->m02, i->m12, i->m22);
  else if(pos == 3)
    vec3_set(o, i->m03, i->m13, i->m23);
  else
    vec3_set(o, 0.0, 0.0, 0.0);
}

/* get a row of a matrix, 4 elements */
void
mat4_cpy_v4row( vec4 *o, const mat4 *i, const int pos )
{
  if(pos == 0)
    vec4_set(o, i->m00, i->m10, i->m20, i->m30);
  else if(pos == 1)
    vec4_set(o, i->m01, i->m11, i->m21, i->m31);
  else if(pos == 2)
    vec4_set(o, i->m02, i->m12, i->m22, i->m32);
  else if(pos == 3)
    vec4_set(o, i->m03, i->m13, i->m23, i->m33);
  else
    vec4_set(o, 0.0, 0.0, 0.0, 0.0);
}

/* get a column of a matrix, 3 elements */
void
mat4_cpy_v3col( vec3 *o, const mat4 *i, const int pos )
{
  if(pos == 0)
    vec3_set(o, i->m00, i->m01, i->m02);
  else if(pos == 1)
    vec3_set(o, i->m10, i->m11, i->m12);
  else if(pos == 2)
    vec3_set(o, i->m20, i->m21, i->m22);
  else if(pos == 3)
    vec3_set(o, i->m30, i->m31, i->m32);
  else
    vec3_set(o, 0.0, 0.0, 0.0);
}

/* get a column of a matrix, 4 elements */
void
mat4_cpy_v4col( vec4 *o, const mat4 *i, const int pos )
{
  if(pos == 0)
    vec4_set(o, i->m00, i->m01, i->m02, i->m03);
  else if(pos == 1)
    vec4_set(o, i->m10, i->m11, i->m12, i->m13);
  else if(pos == 2)
    vec4_set(o, i->m20, i->m21, i->m22, i->m23);
  else if(pos == 3)
    vec4_set(o, i->m30, i->m31, i->m32, i->m33);
  else
    vec4_set(o, 0.0f, 0.0f, 0.0f, 0.0f);
}

/* add 2 matrices */
void
mat4_add( mat4 *o, const mat4 *i1, const mat4 *i2 )
{
  o->m[ 0] = i1->m[ 0] + i2->m[ 0];
  o->m[ 1] = i1->m[ 1] + i2->m[ 1];
  o->m[ 2] = i1->m[ 2] + i2->m[ 2];
  o->m[ 3] = i1->m[ 3] + i2->m[ 3];
  o->m[ 4] = i1->m[ 4] + i2->m[ 4];
  o->m[ 5] = i1->m[ 5] + i2->m[ 5];
  o->m[ 6] = i1->m[ 6] + i2->m[ 6];
  o->m[ 7] = i1->m[ 7] + i2->m[ 7];
  o->m[ 8] = i1->m[ 8] + i2->m[ 8];
  o->m[ 9] = i1->m[ 9] + i2->m[ 9];
  o->m[10] = i1->m[10] + i2->m[10];
  o->m[11] = i1->m[11] + i2->m[11];
  o->m[12] = i1->m[12] + i2->m[12];
  o->m[13] = i1->m[13] + i2->m[13];
  o->m[14] = i1->m[14] + i2->m[14];
  o->m[15] = i1->m[15] + i2->m[15];
}

/* subtract 1 matrix from another matrix */
void
mat4_sub( mat4 *o, const mat4 *i1, const mat4 *i2 )
{
  o->m[ 0] = i1->m[ 0] - i2->m[ 0];
  o->m[ 1] = i1->m[ 1] - i2->m[ 1];
  o->m[ 2] = i1->m[ 2] - i2->m[ 2];
  o->m[ 3] = i1->m[ 3] - i2->m[ 3];
  o->m[ 4] = i1->m[ 4] - i2->m[ 4];
  o->m[ 5] = i1->m[ 5] - i2->m[ 5];
  o->m[ 6] = i1->m[ 6] - i2->m[ 6];
  o->m[ 7] = i1->m[ 7] - i2->m[ 7];
  o->m[ 8] = i1->m[ 8] - i2->m[ 8];
  o->m[ 9] = i1->m[ 9] - i2->m[ 9];
  o->m[10] = i1->m[10] - i2->m[10];
  o->m[11] = i1->m[11] - i2->m[11];
  o->m[12] = i1->m[12] - i2->m[12];
  o->m[13] = i1->m[13] - i2->m[13];
  o->m[14] = i1->m[14] - i2->m[14];
  o->m[15] = i1->m[15] - i2->m[15];
}

/* prepare a scale matrix by scale factor */
void
mat4_scale( mat4 *o, const mat4 *i, const float s )
{
  o->m00 = i->m00*s;
  o->m11 = i->m11*s;
  o->m22 = i->m22*s;
}

/* prepare a scale matrix by scale factor */
void
mat4_self_scale( mat4 *o, const float s )
{
  o->m00 *= s;
  o->m11 *= s;
  o->m22 *= s;
}

/* multiply 2 matrices */
void
mat4_mul( mat4 *o, const mat4 *i1, const mat4 *i2 )
{
  /* Optimise for both i1, i2 matrices - bottom row is ( 0, 0, 0, 1 ) */
  if(i1->m03 == 0.0 && i1->m13 == 0.0 && i1->m23 == 0.0 && i1->m33 == 1.0 &&
     i2->m03 == 0.0 && i2->m13 == 0.0 && i2->m23 == 0.0 && i2->m33 == 1.0) {
    o->m00 = i1->m00*i2->m00 + i1->m10*i2->m01 + i1->m20*i2->m02;
    o->m01 = i1->m01*i2->m00 + i1->m11*i2->m01 + i1->m21*i2->m02;
    o->m02 = i1->m02*i2->m00 + i1->m12*i2->m01 + i1->m22*i2->m02;
    o->m03 = 0.0;

    o->m10 = i1->m00*i2->m10 + i1->m10*i2->m11 + i1->m20*i2->m12;
    o->m11 = i1->m01*i2->m10 + i1->m11*i2->m11 + i1->m21*i2->m12;
    o->m12 = i1->m02*i2->m10 + i1->m12*i2->m11 + i1->m22*i2->m12;
    o->m13 = 0.0;

    o->m20 = i1->m00*i2->m20 + i1->m10*i2->m21 + i1->m20*i2->m22;
    o->m21 = i1->m01*i2->m20 + i1->m11*i2->m21 + i1->m21*i2->m22;
    o->m22 = i1->m02*i2->m20 + i1->m12*i2->m21 + i1->m22*i2->m22;
    o->m23 = 0.0;

    o->m30 = i1->m00*i2->m30 + i1->m10*i2->m31 + i1->m20*i2->m32 + i1->m30;
    o->m31 = i1->m01*i2->m30 + i1->m11*i2->m31 + i1->m21*i2->m32 + i1->m31;
    o->m32 = i1->m02*i2->m30 + i1->m12*i2->m31 + i1->m22*i2->m32 + i1->m32;
    o->m33 = 1.0;
  }
  /* Optimise for when bottom row of i1(1st) matrix is ( 0, 0, 0, 1 ) */
  else if(i1->m03 == 0.0 && i1->m13 == 0.0 && i1->m23 == 0.0 && i1->m33 == 1.0) {	   
    o->m00 = i1->m00*i2->m00 + i1->m10*i2->m01 + i1->m20*i2->m02 + i1->m30*i2->m03;
    o->m01 = i1->m01*i2->m00 + i1->m11*i2->m01 + i1->m21*i2->m02 + i1->m31*i2->m03;
    o->m02 = i1->m02*i2->m00 + i1->m12*i2->m01 + i1->m22*i2->m02 + i1->m32*i2->m03;
    o->m03 = i2->m03;

    o->m10 = i1->m00*i2->m10 + i1->m10*i2->m11 + i1->m20*i2->m12 + i1->m30*i2->m13;
    o->m11 = i1->m01*i2->m10 + i1->m11*i2->m11 + i1->m21*i2->m12 + i1->m31*i2->m13;
    o->m12 = i1->m02*i2->m10 + i1->m12*i2->m11 + i1->m22*i2->m12 + i1->m32*i2->m13;
    o->m13 = i2->m13;

    o->m20 = i1->m00*i2->m20 + i1->m10*i2->m21 + i1->m20*i2->m22 + i1->m30*i2->m23;
    o->m21 = i1->m01*i2->m20 + i1->m11*i2->m21 + i1->m21*i2->m22 + i1->m31*i2->m23;
    o->m22 = i1->m02*i2->m20 + i1->m12*i2->m21 + i1->m22*i2->m22 + i1->m32*i2->m23;
    o->m23 = i2->m23;

    o->m30 = i1->m00*i2->m30 + i1->m10*i2->m31 + i1->m20*i2->m32 + i1->m30*i2->m33;
    o->m31 = i1->m01*i2->m30 + i1->m11*i2->m31 + i1->m21*i2->m32 + i1->m31*i2->m33;
    o->m32 = i1->m02*i2->m30 + i1->m12*i2->m31 + i1->m22*i2->m32 + i1->m31*i2->m33;
    o->m33 = i2->m33;
  }
  /* Optimise for when bottom row of i2(2nd) matrix is ( 0, 0, 0, 1 ) */
  else if(i2->m03 == 0.0 && i2->m13 == 0.0 && i2->m23 == 0.0 && i2->m33 == 1.0) {
    o->m00 = i1->m00*i2->m00 + i1->m10*i2->m01 + i1->m20*i2->m02;
    o->m01 = i1->m01*i2->m00 + i1->m11*i2->m01 + i1->m21*i2->m02;
    o->m02 = i1->m02*i2->m00 + i1->m12*i2->m01 + i1->m22*i2->m02;
    o->m03 = i1->m03*i2->m00 + i1->m13*i2->m01 + i1->m23*i2->m02;

    o->m10 = i1->m00*i2->m10 + i1->m10*i2->m11 + i1->m20*i2->m12;
    o->m11 = i1->m01*i2->m10 + i1->m11*i2->m11 + i1->m21*i2->m12;
    o->m12 = i1->m02*i2->m10 + i1->m12*i2->m11 + i1->m22*i2->m12;
    o->m13 = i1->m03*i2->m10 + i1->m13*i2->m11 + i1->m23*i2->m12;

    o->m20 = i1->m00*i2->m20 + i1->m10*i2->m21 + i1->m20*i2->m22;
    o->m21 = i1->m01*i2->m20 + i1->m11*i2->m21 + i1->m21*i2->m22;
    o->m22 = i1->m02*i2->m20 + i1->m12*i2->m21 + i1->m22*i2->m22;
    o->m23 = i1->m03*i2->m20 + i1->m13*i2->m21 + i1->m23*i2->m22;

    o->m30 = i1->m00*i2->m30 + i1->m10*i2->m31 + i1->m20*i2->m32 + i1->m30;
    o->m31 = i1->m01*i2->m30 + i1->m11*i2->m31 + i1->m21*i2->m32 + i1->m31;
    o->m32 = i1->m02*i2->m30 + i1->m12*i2->m31 + i1->m22*i2->m32 + i1->m32;
    o->m33 = i1->m03*i2->m30 + i1->m13*i2->m31 + i1->m23*i2->m32 + i1->m33;
  }
  else {
    o->m00 = i1->m00*i2->m00 + i1->m10*i2->m01 + i1->m20*i2->m02 + i1->m30*i2->m03;
    o->m01 = i1->m01*i2->m00 + i1->m11*i2->m01 + i1->m21*i2->m02 + i1->m31*i2->m03;
    o->m02 = i1->m02*i2->m00 + i1->m12*i2->m01 + i1->m22*i2->m02 + i1->m32*i2->m03;
    o->m03 = i1->m03*i2->m00 + i1->m13*i2->m01 + i1->m23*i2->m02 + i1->m33*i2->m03;

    o->m10 = i1->m00*i2->m10 + i1->m10*i2->m11 + i1->m20*i2->m12 + i1->m30*i2->m13;
    o->m11 = i1->m01*i2->m10 + i1->m11*i2->m11 + i1->m21*i2->m12 + i1->m31*i2->m13;
    o->m12 = i1->m02*i2->m10 + i1->m12*i2->m11 + i1->m22*i2->m12 + i1->m32*i2->m13;
    o->m13 = i1->m03*i2->m10 + i1->m13*i2->m11 + i1->m23*i2->m12 + i1->m33*i2->m13;

    o->m20 = i1->m00*i2->m20 + i1->m10*i2->m21 + i1->m20*i2->m22 + i1->m30*i2->m23;
    o->m21 = i1->m01*i2->m20 + i1->m11*i2->m21 + i1->m21*i2->m22 + i1->m31*i2->m23;
    o->m22 = i1->m02*i2->m20 + i1->m12*i2->m21 + i1->m22*i2->m22 + i1->m32*i2->m23;
    o->m23 = i1->m03*i2->m20 + i1->m13*i2->m21 + i1->m23*i2->m22 + i1->m33*i2->m23;

    o->m30 = i1->m00*i2->m30 + i1->m10*i2->m31 + i1->m20*i2->m32 + i1->m30*i2->m33;
    o->m31 = i1->m01*i2->m30 + i1->m11*i2->m31 + i1->m21*i2->m32 + i1->m31*i2->m33;
    o->m32 = i1->m02*i2->m30 + i1->m12*i2->m31 + i1->m22*i2->m32 + i1->m32*i2->m33;
    o->m33 = i1->m03*i2->m30 + i1->m13*i2->m31 + i1->m23*i2->m32 + i1->m33*i2->m33;
  }
}

/* get a 16-element array flipped on the main diagonal */
void
mat4_transpose( mat4 *o, const mat4 *i )
{
  o->m00 = i->m00;
  o->m01 = i->m10;
  o->m02 = i->m20;
  o->m03 = i->m30;

  o->m10 = i->m01;
  o->m11 = i->m11;
  o->m12 = i->m21;
  o->m13 = i->m31;

  o->m20 = i->m02;
  o->m21 = i->m12;
  o->m22 = i->m22;
  o->m23 = i->m32;

  o->m30 = i->m03;
  o->m31 = i->m13;
  o->m32 = i->m23;
  o->m33 = i->m33;
}

/* returns a scalar value with the determinant for mat4 */
float
mat4_determinant( const mat4 *i )
{
  float det;

  det = i->m30*i->m21*i->m12*i->m03 -
        i->m20*i->m31*i->m12*i->m03 -
        i->m30*i->m11*i->m22*i->m03 +
        i->m10*i->m31*i->m22*i->m03 +
        i->m20*i->m11*i->m32*i->m03 -
        i->m10*i->m21*i->m32*i->m03 -
        i->m30*i->m21*i->m02*i->m13 +
        i->m20*i->m31*i->m02*i->m13 +
        i->m30*i->m01*i->m22*i->m13 -
        i->m00*i->m31*i->m22*i->m13 -
        i->m20*i->m01*i->m32*i->m13 +
        i->m00*i->m21*i->m32*i->m13 +
        i->m30*i->m11*i->m02*i->m23 -
        i->m10*i->m31*i->m02*i->m23 -
        i->m30*i->m01*i->m12*i->m23 +
        i->m00*i->m31*i->m12*i->m23 +
        i->m10*i->m01*i->m32*i->m23 -
        i->m00*i->m11*i->m32*i->m23 -
        i->m20*i->m11*i->m02*i->m33 +
        i->m10*i->m21*i->m02*i->m33 +
        i->m20*i->m01*i->m12*i->m33 -
        i->m00*i->m21*i->m12*i->m33 -
        i->m10*i->m01*i->m22*i->m33 +
        i->m00*i->m11*i->m22*i->m33;

  return det;
}

/* inverse a mat4 matrix - general mathematical way */
void
mat4_inv_math( mat4 *o, const mat4 *i )
{
  float det;

  det = mat4_determinant(i);
  /* if cannot inverse, make it identity matrix */
  if(fabs(det) <= EPSILON) {
    mat4_identity(o);
    return;
  }

  float idet = 1.0/det;
  o->m00 = (i->m12*i->m23*i->m31 - i->m13*i->m22*i->m31 +
            i->m13*i->m21*i->m32 - i->m11*i->m23*i->m32 -
            i->m12*i->m21*i->m33 + i->m11*i->m22*i->m33)*idet;
  o->m01 = (i->m03*i->m22*i->m31 - i->m02*i->m23*i->m31 -
            i->m03*i->m21*i->m32 + i->m01*i->m23*i->m32 +
            i->m02*i->m21*i->m33 - i->m01*i->m22*i->m33)*idet;
  o->m02 = (i->m02*i->m13*i->m31 - i->m03*i->m12*i->m31 +
            i->m03*i->m11*i->m32 - i->m01*i->m13*i->m32 -
            i->m02*i->m11*i->m33 + i->m01*i->m12*i->m33)*idet;
  o->m03 = (i->m03*i->m12*i->m21 - i->m02*i->m13*i->m21 -
            i->m03*i->m11*i->m22 + i->m01*i->m13*i->m22 +
            i->m02*i->m11*i->m23 - i->m01*i->m12*i->m23)*idet;

  o->m10 = (i->m13*i->m22*i->m30 - i->m12*i->m23*i->m30 -
            i->m13*i->m20*i->m32 + i->m10*i->m23*i->m32 +
            i->m12*i->m20*i->m33 - i->m10*i->m22*i->m33)*idet;
  o->m11 = (i->m02*i->m23*i->m30 - i->m03*i->m22*i->m30 +
            i->m03*i->m20*i->m32 - i->m00*i->m23*i->m32 -
            i->m02*i->m20*i->m33 + i->m00*i->m22*i->m33)*idet;
  o->m12 = (i->m03*i->m12*i->m30 - i->m02*i->m13*i->m30 -
            i->m03*i->m10*i->m32 + i->m00*i->m13*i->m32 +
            i->m02*i->m10*i->m33 - i->m00*i->m12*i->m33)*idet;
  o->m13 = (i->m02*i->m13*i->m20 - i->m03*i->m12*i->m20 +
            i->m03*i->m10*i->m22 - i->m00*i->m13*i->m22 -
            i->m02*i->m10*i->m23 + i->m00*i->m12*i->m23)*idet;

  o->m20 = (i->m11*i->m23*i->m30 - i->m13*i->m21*i->m30 +
            i->m13*i->m20*i->m31 - i->m10*i->m23*i->m31 -
            i->m11*i->m20*i->m33 + i->m10*i->m21*i->m33)*idet;
  o->m21 = (i->m03*i->m21*i->m30 - i->m01*i->m23*i->m30 -
            i->m03*i->m20*i->m31 + i->m00*i->m23*i->m31 +
            i->m01*i->m20*i->m33 - i->m00*i->m21*i->m33)*idet;
  o->m22 = (i->m01*i->m13*i->m30 - i->m03*i->m11*i->m30 +
            i->m03*i->m10*i->m31 - i->m00*i->m13*i->m31 -
            i->m01*i->m10*i->m33 + i->m00*i->m11*i->m33)*idet;
  o->m23 = (i->m03*i->m11*i->m20 - i->m01*i->m13*i->m20 -
            i->m03*i->m10*i->m21 + i->m00*i->m13*i->m21 +
            i->m01*i->m10*i->m23 - i->m00*i->m11*i->m23)*idet;

  o->m30 = (i->m12*i->m21*i->m30 - i->m11*i->m22*i->m30 -
            i->m12*i->m20*i->m31 + i->m10*i->m22*i->m31 +
            i->m11*i->m20*i->m32 - i->m10*i->m21*i->m32)*idet;
  o->m31 = (i->m01*i->m22*i->m30 - i->m02*i->m21*i->m30 +
            i->m02*i->m20*i->m31 - i->m00*i->m22*i->m31 -
            i->m01*i->m20*i->m32 + i->m00*i->m21*i->m32)*idet;
  o->m32 = (i->m02*i->m11*i->m30 - i->m01*i->m12*i->m30 -
            i->m02*i->m10*i->m31 + i->m00*i->m12*i->m31 +
            i->m01*i->m10*i->m32 - i->m00*i->m11*i->m32)*idet;
  o->m33 = (i->m01*i->m12*i->m20 - i->m02*i->m11*i->m20 +
            i->m02*i->m10*i->m21 - i->m00*i->m12*i->m21 -
            i->m01*i->m10*i->m22 + i->m00*i->m11*i->m22)*idet;
}

/*-----------------------------------------------------------------------*/
/*  compute cofactor of 3x3 minor matrix without sign                    */
/*  input params are 9 elements of the minor matrix                      */
/*  NOTE: The caller must know its sign                                  */
/*                                                                       */
/* static float                                                          */
/* get_cofactor( float m0, float m1, float m2,                           */
/*               float m3, float m4, float m5,                           */
/*               float m6, float m7, float m8 )                          */
/* {                                                                     */
/*   return m0*(m4*m8 - m5*m7) -                                         */
/*          m1*(m3*m8 - m5*m6) +                                         */
/*          m2*(m3*m7 - m4*m6);                                          */
/* }                                                                     */
/*                                                                       */
/* compute the inverse of a general 4x4 matrix using Cramer's Rule       */
/* If cannot find inverse, return indentity matrix                       */
/* M^-1 = adj( M )/det( M )                                              */
/*                                                                       */
/* static void                                                           */
/* mat4_inv( mat4 *o, const mat4 *i )                                    */
/* {                                                                     */
/*   get cofactors of minor matrices                                     */
/*   float cof00 = get_cofactor(i->m[ 5], i->m[ 9], i->m[13],            */
/*                              i->m[ 6], i->m[10], i->m[14],            */
/*                              i->m[ 7], i->m[11], i->m[15]);           */
/*   float cof01 = get_cofactor(i->m[ 4], i->m[ 8], i->m[12],            */
/*                              i->m[ 6], i->m[10], i->m[14],            */
/*                              i->m[ 7], i->m[11], i->m[15]);           */
/*   float cof02 = get_cofactor(i->m[ 4], i->m[ 8], i->m[12],            */
/*                              i->m[ 5], i->m[ 9], i->m[13],            */
/*                              i->m[ 7], i->m[11], i->m[15]);           */
/*   float cof03 = get_cofactor(i->m[ 4], i->m[ 8], i->m[12],            */
/*                              i->m[ 5], i->m[ 9], i->m[13],            */
/*                              i->m[ 6], i->m[10], i->m[14]);           */
/*                                                                       */
/*   get determinant                                                     */
/*   float det = i->m[ 0]*cof00 -                                        */
/*               i->m[ 4]*cof01 +                                        */
/*               i->m[ 8]*cof02 -                                        */
/*               i->m[12]*cof03;                                         */
/*   if cannot inverse, make it identity matrix                          */
/*   if(fabs(det) <= EPSILON) {                                          */
/*     mat4_identity(m);                                                 */
/*     return;                                                           */
/*   }                                                                   */
/*                                                                       */
/*   get rest of cofactors for adj(M)                                    */
/*   float cof04 = get_cofactor(i->m[ 1], i->m[ 9], i->m[13],            */
/*                              i->m[ 2], i->m[10], i->m[14],            */
/*                              i->m[ 3], i->m[11], i->m[15]);           */
/*   float cof05 = get_cofactor(i->m[ 0], i->m[ 8], i->m[12],            */
/*                              i->m[ 2], i->m[10], i->m[14],            */
/*                              i->m[ 3], i->m[11], i->m[15]);           */
/*   float cof06 = get_cofactor(i->m[ 0], i->m[ 8], i->m[12],            */
/*                              i->m[ 1], i->m[ 9], i->m[13],            */
/*                              i->m[ 3], i->m[11], i->m[15]);           */
/*   float cof07 = get_cofactor(i->m[ 0], i->m[ 8], i->m[12],            */
/*                              i->m[ 1], i->m[ 9], i->m[13],            */
/*                              i->m[ 2], i->m[10], i->m[14]);           */
/*                                                                       */
/*   float cof08 = get_cofactor(i->m[ 1], i->m[ 5], i->m[13],            */
/*                              i->m[ 2], i->m[ 6], i->m[14],            */
/*                              i->m[ 3], i->m[ 7], i->m[15]);           */
/*   float cof09 = get_cofactor(i->m[ 0], i->m[ 4], i->m[12],            */
/*                              i->m[ 2], i->m[ 6], i->m[14],            */
/*                              i->m[ 3], i->m[ 7], i->m[15]);           */
/*   float cof10 = get_cofactor(i->m[ 0], i->m[ 4], i->m[12],            */
/*                              i->m[ 1], i->m[ 5], i->m[13],            */
/*                              i->m[ 3], i->m[ 7], i->m[15]);           */
/*   float cof11 = get_cofactor(i->m[ 0], i->m[ 4], i->m[12],            */
/*                              i->m[ 1], i->m[ 5], i->m[13],            */
/*                              i->m[ 2], i->m[ 6], i->m[14]);           */
/*                                                                       */
/*   float cof12 = get_cofactor(i->m[ 1], i->m[ 5], i->m[ 9],            */
/*                              i->m[ 2], i->m[ 6], i->m[10],            */
/*                              i->m[ 3], i->m[ 7], i->m[11]);           */
/*   float cof13 = get_cofactor(i->m[ 0], i->m[ 4], i->m[ 8],            */
/*                              i->m[ 2], i->m[ 6], i->m[10],            */
/*                              i->m[ 3], i->m[ 7], i->m[11]);           */
/*   float cof14 = get_cofactor(i->m[ 0], i->m[ 4], i->m[ 8],            */
/*                              i->m[ 1], i->m[ 5], i->m[ 9],            */
/*                              i->m[ 3], i->m[ 7], i->m[11]);           */
/*   float cof15 = get_cofactor(i->m[ 0], i->m[ 4], i->m[ 8],            */
/*                              i->m[ 1], i->m[ 5], i->m[ 9],            */
/*                              i->m[ 2], i->m[ 6], i->m[10]);           */
/*                                                                       */
/*   build inverse matrix = adj( M )/det( M )                            */
/*   adjugate of M is the transpose of the cofactor matrix of M          */
/*   float idet = 1.0/det;                                               */
/*   o->m[ 0] =  idet*cof00;                                             */
/*   o->m[ 1] = -idet*cof04;                                             */
/*   o->m[ 2] =  idet*cof08;                                             */
/*   o->m[ 3] = -idet*cof12;                                             */
/*                                                                       */
/*   o->m[ 4] = -idet*cof01;                                             */
/*   o->m[ 5] =  idet*cof05;                                             */
/*   o->m[ 6] = -idet*cof09;                                             */
/*   o->m[ 7] =  idet*cof13;                                             */
/*                                                                       */
/*   o->m[ 8] =  idet*cof02;                                             */
/*   o->m[ 9] = -idet*cof06;                                             */
/*   o->m[10] =  idet*cof10;                                             */
/*   o->m[11] = -idet*cof14;                                             */
/*                                                                       */
/*   o->m[12] = -idet*cof03;                                             */
/*   o->m[13] =  idet*cof07;                                             */
/*   o->m[14] = -idet*cof11;                                             */
/*   o->m[15] =  idet*cof15;                                             */
/* }                                                                     */
/*-----------------------------------------------------------------------*/

/* inverse a general mat4 matrix - using Cramer's Rule, optimized a bit */
void
mat4_inv( mat4 *o, const mat4 *i )
{
  float det;
  float c00, c01, c02, c03;   /* column 0 */
  float c10, c11, c12, c13;   /* column 1 */
  float c20, c21, c22, c23;   /* column 2 */
  float c30, c31, c32, c33;   /* column 3 */

  c00 = i->m12*i->m23*i->m31 - i->m13*i->m22*i->m31 +
        i->m13*i->m21*i->m32 - i->m11*i->m23*i->m32 -
        i->m12*i->m21*i->m33 + i->m11*i->m22*i->m33;

  c10 = i->m13*i->m22*i->m30 - i->m12*i->m23*i->m30 -
        i->m13*i->m20*i->m32 + i->m10*i->m23*i->m32 +
        i->m12*i->m20*i->m33 - i->m10*i->m22*i->m33;

  c20 = i->m11*i->m23*i->m30 - i->m13*i->m21*i->m30 +
        i->m13*i->m20*i->m31 - i->m10*i->m23*i->m31 -
        i->m11*i->m20*i->m33 + i->m10*i->m21*i->m33;

  c30 = i->m12*i->m21*i->m30 - i->m11*i->m22*i->m30 -
        i->m12*i->m20*i->m31 + i->m10*i->m22*i->m31 +
        i->m11*i->m20*i->m32 - i->m10*i->m21*i->m32;

  det = i->m00*c00 + i->m01*c10 + i->m02*c20 + i->m03*c30;
  /* if cannot inverse, make it identity matrix */
  if(fabs(det) <= EPSILON) {
    mat4_identity(o);
    return;
  }

  c01 = i->m03*i->m22*i->m31 - i->m02*i->m23*i->m31 -
        i->m03*i->m21*i->m32 + i->m01*i->m23*i->m32 +
        i->m02*i->m21*i->m33 - i->m01*i->m22*i->m33;

  c11 = i->m02*i->m23*i->m30 - i->m03*i->m22*i->m30 +
        i->m03*i->m20*i->m32 - i->m00*i->m23*i->m32 -
        i->m02*i->m20*i->m33 + i->m00*i->m22*i->m33;

  c21 = i->m03*i->m21*i->m30 - i->m01*i->m23*i->m30 -
        i->m03*i->m20*i->m31 + i->m00*i->m23*i->m31 +
        i->m01*i->m20*i->m33 - i->m00*i->m21*i->m33;

  c31 = i->m01*i->m22*i->m30 - i->m02*i->m21*i->m30 +
        i->m02*i->m20*i->m31 - i->m00*i->m22*i->m31 -
        i->m01*i->m20*i->m32 + i->m00*i->m21*i->m32;

  c02 = i->m02*i->m13*i->m31 - i->m03*i->m12*i->m31 +
        i->m03*i->m11*i->m32 - i->m01*i->m13*i->m32 -
        i->m02*i->m11*i->m33 + i->m01*i->m12*i->m33;

  c12 = i->m03*i->m12*i->m30 - i->m02*i->m13*i->m30 -
        i->m03*i->m10*i->m32 + i->m00*i->m13*i->m32 +
        i->m02*i->m10*i->m33 - i->m00*i->m12*i->m33;

  c22 = i->m01*i->m13*i->m30 - i->m03*i->m11*i->m30 +
        i->m03*i->m10*i->m31 - i->m00*i->m13*i->m31 -
        i->m01*i->m10*i->m33 + i->m00*i->m11*i->m33;

  c32 = i->m02*i->m11*i->m30 - i->m01*i->m12*i->m30 -
        i->m02*i->m10*i->m31 + i->m00*i->m12*i->m31 +
        i->m01*i->m10*i->m32 - i->m00*i->m11*i->m32;

  c03 = i->m03*i->m12*i->m21 - i->m02*i->m13*i->m21 -
        i->m03*i->m11*i->m22 + i->m01*i->m13*i->m22 +
        i->m02*i->m11*i->m23 - i->m01*i->m12*i->m23;

  c13 = i->m02*i->m13*i->m20 - i->m03*i->m12*i->m20 +
        i->m03*i->m10*i->m22 - i->m00*i->m13*i->m22 -
        i->m02*i->m10*i->m23 + i->m00*i->m12*i->m23;

  c23 = i->m03*i->m11*i->m20 - i->m01*i->m13*i->m20 -
        i->m03*i->m10*i->m21 + i->m00*i->m13*i->m21 +
        i->m01*i->m10*i->m23 - i->m00*i->m11*i->m23;

  c33 = i->m01*i->m12*i->m20 - i->m02*i->m11*i->m20 +
        i->m02*i->m10*i->m21 - i->m00*i->m12*i->m21 -
        i->m01*i->m10*i->m22 + i->m00*i->m11*i->m22;

  float idet = 1.0/det;
  o->m00 = c00*idet;
  o->m01 = c01*idet;
  o->m02 = c02*idet;
  o->m03 = c03*idet;
  o->m10 = c10*idet;
  o->m11 = c11*idet;
  o->m12 = c12*idet;
  o->m13 = c13*idet;
  o->m20 = c20*idet;
  o->m21 = c21*idet;
  o->m22 = c22*idet;
  o->m23 = c23*idet;
  o->m30 = c30*idet;
  o->m31 = c31*idet;
  o->m32 = c32*idet;
  o->m33 = c33*idet;
}

/*------------------- Affine mat4 functions -------------------------------*/
/* prepare a 4d matrix for translating with xyz array */
void
mat4_set_trans( mat4 *o, const float x, const float y, const float z )
{
  o->m30 = x;
  o->m31 = y;
  o->m32 = z;
}

/* prepare a 4d matrix for rotating around x axis by an angle in radians */
void
mat4_set_x_rot( mat4 *o, const float rad )
{
  float c, s;

  c = cosf(rad);
  s = sinf(rad);	

  o->m11 =  c;
  o->m12 =  s;
  o->m21 = -s;
  o->m22 =  c;
}

/* prepare a 4d matrix for rotating around y axis by an angle in radians */
void
mat4_set_y_rot( mat4 *o, const float rad )
{
  float c, s;

  c = cosf(rad);
  s = sinf(rad);

  o->m00 =  c;
  o->m02 = -s;
  o->m20 =  s;
	o->m22 =  c;
}

/* prepare a 4d matrix for rotating around z axis by an angle in radians */
void
mat4_set_z_rot( mat4 *o, const float rad )
{
  float c, s;

  c = cosf(rad);
  s = sinf(rad);

  o->m00 =  c;
  o->m01 =  s;
  o->m10 = -s;
  o->m11 =  c;
}

/* prepare a 4d matrix for rotate from euler angles, the angles are in radians */
void
mat4_set_rot( mat4 *o, const float rad_x, const float rad_y, const float rad_z )
{
  float cr, sr, cp, sp, cy, sy;
  float srsp, crsp;

  cr = cosf(rad_x);    /* cr - cos roll */
  sr = sinf(rad_x);    /* sr - sin roll */
  cp = cosf(rad_y);    /* cp - cos pitch */
  sp = sinf(rad_y);    /* sp - sin pitch */
  cy = cosf(rad_z);    /* cy - cos yaw */
  sy = sinf(rad_z);    /* sy - sin yaw */

  o->m00 = cp*cy;
  o->m01 = cp*sy;
  o->m02 = -sp;

  srsp = sr*sp;
  crsp = cr*sp;

  o->m10 = srsp*cy - cr*sy;
  o->m11 = srsp*sy + cr*cy;
  o->m12 = sr*cp;

  o->m20 = crsp*cy + sr*sy;
  o->m21 = crsp*sy - sr*cy;
  o->m22 = cr*cp;
}

/* prepare a 4d matrix for rotating around an arbitrary axis by an angle in radians */
void
mat4_set_axis_rot( mat4 *o, const vec3 *axis, const float rad )
{
  float x, y, z, c, s;

  c = cosf(rad);
  s = sinf(rad);
  x = axis->x;
  y = axis->y;
  z = axis->z;

  o->m00 = x*x*(1.0 - c) + c;
  o->m01 = x*y*(1.0 - c) + z*s;
  o->m02 = x*z*(1.0 - c) - y*s;

  o->m10 = y*x*(1.0 - c) - z*s;
  o->m11 = y*y*(1.0 - c) + c;
  o->m12 = y*z*(1.0 - c) + x*s;

  o->m20 = z*x*(1.0 - c) + y*s;
  o->m21 = z*y*(1.0 - c) - x*s;
  o->m22 = z*z*(1.0 - c) + c;
}

/*----------------------------------------------------------------------------*/
/* compute the inverse of 4x4 Euclidean transformation matrix                 */
/*                                                                            */
/* Euclidean transformation is translation, rotation, and reflection.         */
/* With Euclidean transform, only the position and orientation of the object  */
/* will be changed. Euclidean transform doesn't change the shape of an object */
/* (no scaling). Length and angle are reserved.                               */
/*                                                                            */
/* Use mat4_inv_affine() if the matrix has scale and shear transformation.    */
/*                                                                            */
/* M = [ R | T ]                                                              */
/*     [ --+-- ]    (R denotes 3x3 rotation/reflection matrix)                */
/*     [ 0 | 1 ]    (T denotes 1x3 translation matrix)                        */
/*                                                                            */
/* y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^T*y - R^T*T      */
/* (R is orthogonal,  R^-1 = R^T)                                             */
/*                                                                            */
/*  [ R | T ]-1    [ R^T | -R^T*T   ]    (R denotes 3x3 rotation matrix)      */
/*  [ --+-- ]   =  [ ----+--------- ]    (T denotes 1x3 translation)          */
/*  [ 0 | 1 ]      [  0  |     1    ]    (R^T denotes R-transpose)            */
/*----------------------------------------------------------------------------*/
void
mat4_inv_euclidean( mat4 *o, const mat4 *i )
{
  /* transpose 3x3 rotation matrix part */
  /* | R^T | 0 |                        */
  /* | ----+-- |                        */
  /* |  0  | 1 |                        */
  o->m00 = i->m00;
  o->m11 = i->m11;
  o->m22 = i->m22;
  o->m01 = i->m10;  o->m10 = i->m01;
  o->m02 = i->m20;  o->m20 = i->m02;
  o->m12 = i->m21;  o->m21 = i->m12;

  /* compute translation part -R^T*T    */
  /* | 0 | -R^T x |                     */
  /* | --+------- |                     */
  /* | 0 |   0    |                     */
  o->m30 = -(i->m00*i->m30 + i->m01*i->m31 + i->m02*i->m32);
  o->m31 = -(i->m10*i->m30 + i->m11*i->m31 + i->m12*i->m32);
  o->m32 = -(i->m20*i->m30 + i->m21*i->m31 + i->m22*i->m32);

  /* last row should be unchanged (0, 0, 0, 1) */
  o->m03 = o->m13 = o->m23 = 0.0;
  o->m33 = 1.0;
}

/*----------------------------------------------------------------------------*/
/* compute the inverse of a 4x4 affine transformation matrix                  */
/*                                                                            */
/* Affine transformations are generalizations of Euclidean transformations.   */
/* Affine transformation includes translation, rotation, reflection, scaling  */
/* and shearing. Length and angle are NOT preserved.                          */
/* M = [ R | T ]                                                              */
/*     [ --+-- ]    (R denotes 3x3 rotation/scale/shear matrix)               */
/*     [ 0 | 1 ]    (T denotes 1x3 translation matrix)                        */
/*                                                                            */
/* y = M*x  ->  y = R*x + T  ->  x = R^-1*(y - T)  ->  x = R^-1*y - R^-1*T    */
/*                                                                            */
/*    [ R | T ]-1   [ R^-1 | -R^-1 * T ]                                      */
/*    [ --+-- ]   = [ -----+---------- ]                                      */
/*    [ 0 | 1 ]     [  0   +     1     ]                                      */
/*----------------------------------------------------------------------------*/
void
mat4_inv_affine( mat4 *o, const mat4 *i )
{
  /* R^-1 */
  mat3 R, invR;
  mat3_frm_mat4(&R, i);
  mat3_inv(&invR, &R);

  o->m00 = invR.m00;  o->m01 = invR.m01;  o->m02 = invR.m02; /* 1st column */
  o->m10 = invR.m10;  o->m11 = invR.m11;  o->m12 = invR.m12; /* 2nd column */
  o->m20 = invR.m20;  o->m21 = invR.m21;  o->m22 = invR.m22; /* 3rd column */

  /* -R^-1 * T */
  o->m30  = -(invR.m00*i->m30 + invR.m10*i->m31 + invR.m20*i->m32);
  o->m31  = -(invR.m01*i->m30 + invR.m11*i->m31 + invR.m21*i->m32);
  o->m32  = -(invR.m02*i->m30 + invR.m12*i->m31 + invR.m22*i->m32);

  /* last row should be unchanged (0, 0, 0, 1) */
  o->m03 = o->m13 = o->m23 = 0.0;
  o->m33 = 1.0;
}

/* prepare a orthographic matrix */
void
mat4_set_ortho( mat4 *o, 
                const float l,  /* left */
                const float r,  /* right */
                const float b,  /* bottom*/
                const float t,  /* top */
                const float n,  /* z-near */
                const float f ) /* z-far */
{
  o->m00 = 2.0/(r - l);
  o->m01 = 0.0;
  o->m02 = 0.0;
  o->m03 = 0.0;

  o->m10 = 0.0;
  o->m11 = 2.0/(t - b);
  o->m12 = 0.0;
  o->m13 = 0.0;

  o->m20 = 0.0;
  o->m21 = 0.0;
  o->m22 = -2.0/(f - n);
  o->m23 = 0.0;

  o->m30 = -(r + l)/(r - l);
  o->m31 = -(t + b)/(t - b);
  o->m32 = -(f + n)/(f - n);
  o->m33 = 1.0;
}

/* prepare a frustum matrix */
void
mat4_set_frustum( mat4 *o,
                  const float l,  /* left   (near) */
                  const float r,  /* right  (near) */
                  const float b,  /* bottom (near)*/
                  const float t,  /* top    (near) */
                  const float n,  /* z-near */
                  const float f ) /* z-far */
{
  mat4_zero(o);

  o->m00 = 2.0*n/(r - l);
  o->m11 = 2.0*n/(t - b);

  o->m20 = (r + l)/(r - l);
  o->m22 = -(f + n)/(f - n);
  o->m23 = -1.0;

  o->m31 = (t + b)/(t - b);
  o->m32 = -2.0*n/(f - n);
}

/* prepare a projection matrix */
void
mat4_set_proj( mat4 *o,
               const float fovy,   /* in degree */
               const float aspect, /* aspect 4:3, 16:9 etc. */
               const float n,      /* z-near */
               const float f )     /* z-far */
{
  float h = 1.0/tanf(DEG2RAD(fovy*0.5));
  float neg_depth = n - f;

  o->m00 = h/aspect;
  o->m01 = 0.0;
  o->m02 = 0.0;
  o->m03 = 0.0;

  o->m10 = 0.0;
  o->m11 = h;
  o->m12 = 0.0;
  o->m13 = 0.0;

  o->m20 = 0.0;
  o->m21 = 0.0;
  o->m22 = (n + f)/neg_depth;
  o->m23 = -1.0f;

  o->m30 = 0.0;
  o->m31 = 0.0;
  o->m32 = 2.0*n*f/neg_depth;
  o->m33 = 0.0;
}

/* prepare a view matrix
   p - camera position
   f - camera's forward axis
   u - camera's up axis
   r - camera's right axis */
void
mat4_set_view( mat4 *o, const vec3 *p,
                        const vec3 *f,
                        const vec3 *u,
                        const vec3 *r )
{
  o->m00 = r->x;
  o->m01 = u->x;
  o->m02 = f->x;
  o->m03 = 0.0;

  o->m10 = r->y;
  o->m11 = u->y;
  o->m12 = f->y;
  o->m13 = 0.0;

  o->m20 = r->z;
  o->m21 = u->z;
  o->m22 = f->z;
  o->m23 = 0.0;

  o->m30 = -vec3_dot(r, p);
  o->m31 = -vec3_dot(u, p);
  o->m32 = -vec3_dot(f, p);

  o->m33 = 1.0;
}

/* prepare a model matrix
   p - object position
   f - object's forward axis
   u - object's up axis
   r - object's right axis */
void
mat4_set_model( mat4 *o, const vec3 *p,
                         const vec3 *f,
                         const vec3 *u,
                         const vec3 *r )
{
  o->m00 = r->x;
  o->m01 = r->y;
  o->m02 = r->z;
  o->m03 = 0.0;

  o->m10 = u->x;
  o->m11 = u->y;
  o->m12 = u->z;
  o->m13 = 0.0;

  o->m20 = f->x;
  o->m21 = f->y;
  o->m22 = f->z;
  o->m23 = 0.0;

  o->m30 = p->x;
  o->m31 = p->y;
  o->m32 = p->z;
  o->m33 = 1.0;
}

/* convert quaternion to a matrix 
void
mat4_from_quat( mat4 *o, const vec4 *i )
{
  float x = i->x;
  float y = i->y;
  float z = i->z;
  float w = i->w;

  mat4_set(o,
           1.0 - 2.0*y*y - 2.0*z*z, 2.0*x*y - 2.0*w*z,       2.0*x*z + 2.0*w*y,       0.0,
		       2.0*x*y + 2.0*w*z,       1.0 - 2.0*x*x - 2.0*z*z, 2.0*y*z - 2.0*w*x,       0.0,
		       2.0*x*z - 2.0*w*y,       2.0*y*z + 2.0f*w*x,      1.0 - 2.0*x*x - 2.0*y*y, 0.0,
		       0.0,                     0.0,                     0.0,                     1.0);
}
*/

/* convert quaternion to a matrix */
void
mat4_from_quat( mat4 *o, const vec4 *i )
{
  float xx, yy, zz, xy, xz, zy, xw, yw, zw;

  xx = i->x*i->x;
  yy = i->y*i->y;
  zz = i->z*i->z;

  xy = i->x*i->y;
  zw = i->z*i->w;
  xz = i->x*i->z;
  yw = i->y*i->w;
  zy = i->z*i->y;
  xw = i->x*i->w;

  o->m00 = 1.0 - 2.0*(yy + zz);
  o->m01 = 2.0*(xy + zw);
  o->m02 = 2.0*(xz - yw);
  o->m03 = 0.0;

  o->m10 = 2.0*(xy - zw);
  o->m11 = 1.0 - 2.0*(xx + zz);
  o->m12 = 2.0*(zy + xw);
  o->m13 = 0.0;

  o->m20 = 2.0*(xz + yw);
  o->m21 = 2.0*(zy - xw);
  o->m22 = 1.0 - 2.0*(xx + yy);
  o->m23 = 0.0;

  o->m30 = 0.0;
  o->m31 = 0.0;
  o->m32 = 0.0;
  o->m33 = 1.0;
}

/* transform a vec4 by a mat4 */
void
mat4_mul_vec4( vec4 *o, const mat4 *m, const vec4 *i )
{
  /* 0x + 4y +  8z + 12w */
  /* 1x + 5y +  9z + 13w */
  /* 2x + 6y + 10z + 14w */
  /* 3x + 7y + 11z + 15w */
  o->x = m->m00*i->x + m->m10*i->y + m->m20*i->z + m->m30*i->w;
  o->y = m->m01*i->x + m->m11*i->y + m->m21*i->z + m->m31*i->w;
  o->z = m->m02*i->x + m->m12*i->y + m->m22*i->z + m->m32*i->w;
  o->w = m->m03*i->x + m->m13*i->y + m->m23*i->z + m->m33*i->w;
}

/* rotate a 3d vector by rotation part of mat4 */
void
mat4_r_mul_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = m->m00*i->x + m->m10*i->y + m->m20*i->z;
  o->y = m->m01*i->x + m->m11*i->y + m->m21*i->z;
  o->z = m->m02*i->x + m->m12*i->y + m->m22*i->z;
}

/* rotate by mat4's rotation part's transpose */
void
mat4_inv_r_mul_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = m->m00*i->x + m->m01*i->y + m->m02*i->z;
  o->y = m->m10*i->x + m->m11*i->y + m->m12*i->z;
  o->z = m->m20*i->x + m->m21*i->y + m->m22*i->z;
}

/* translate a 3d vector by mat4's translation part */
void
mat4_t_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = i->x + m->m30;
  o->y = i->y + m->m31;
  o->z = i->z + m->m32;
}

/* inverse translate a 3d vector by mat4's translation part */
void
mat4_inv_t_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = i->x - m->m30;
  o->y = i->y - m->m31;
  o->z = i->z - m->m32;
}

/* rotate a 3d vector and then translate the 3d vector respectively */
void
mat4_r_mul_t_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = m->m00*i->x + m->m10*i->y + m->m20*i->z + m->m30;
  o->y = m->m01*i->x + m->m11*i->y + m->m21*i->z + m->m31;
  o->z = m->m02*i->x + m->m12*i->y + m->m22*i->z + m->m32;
}

/* inverse rotate a 3d vector and then translate the 3d vector respectively */
void
mat4_inv_r_mul_t_vec3( vec3 *o, const mat4 *m, const vec3 *i )
{
  o->x = m->m00*i->x + m->m01*i->y + m->m02*i->z + m->m30;
  o->y = m->m10*i->x + m->m11*i->y + m->m12*i->z + m->m31;
  o->z = m->m20*i->x + m->m21*i->y + m->m22*i->z + m->m32;
}