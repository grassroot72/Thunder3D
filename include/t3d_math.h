/*----- t3d_math.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_math_h_
#define _t3d_math_h_

#include <string.h>
#include <math.h>
#include <t3d_type.h>


/* convert degrees into radians */
#define DEG2RAD( degree ) ( degree*0.017453293 )  /* M_PI/180.0 = 0.017453293 */

/* An epsilon value for comparisons */
#ifndef EPSILON
#define EPSILON 0.00001
#endif


/* ivec2 is a tuple of 2 integers
   Each field can be addressed using several aliases:
   - 1st component: x, s, start
   - 2nd component: y, t, end */
union __ivec2 {
  int v[2];      /* All compoments at once */
  struct {
    int x;       /* Alias for 1st component */
    int y;       /* Alias for 2nd component */
  };
  struct {
    int s;       /* Alias for 1st component */
    int t;       /* Alias for 2nd component */
  };
  struct {
    int start;   /* Alias for 1st component */
    int end;     /* Alias for 2nd component */
  };
};

/* ivec3 is a tuple of 3 integers
   Each field can be addressed using several aliases:
   - 1st component:  x, r, red
   - 2nd component:  y, g, green
   - 3rd component:  z, b, blue */
union __ivec3 {
  int v[3];      /* All compoments at once */
  struct {
    int x;       /* Alias for 1st component */
    int y;       /* Alias for 2nd component */
    int z;       /* Alias for 3rd component */
  };
  struct {
    int r;       /* Alias for 1st component */
    int g;       /* Alias for 2nd component */
    int b;       /* Alias for 3rd component */
  };
  struct {
    int red;     /* Alias for 1st component */
    int green;   /* Alias for 2nd component */
    int blue;    /* Alias for 3rd component */
  };
};

/* ivec4 is a tuple of 4 integers
   Each field can be addressed using several aliases:
   - 1st component:  x, r, red,   x_,         vstart
   - 2nd component:  y, g, green, y_,         vcount
   - 3rd component:  z, b, blue,  w_,(width)  istart
   - 4th component:  w, a, alpha, h_,(height) icount */
union __ivec4 {
  int v[4];      /* All compoments at once */
  struct {
    int x;       /* Alias for 1st component */
    int y;       /* Alias for 2nd component */
    int z;       /* Alias for 3rd component */
    int w;       /* Alias for 4th component */
  };
  struct {
    int x_;      /* Alias for 1st component */
    int y_;      /* Alias for 2nd component */
    int w_;      /* Alias for 3rd component */
    int h_;      /* Alias for 4th component */
  };
  struct {
    int r;       /* Alias for 1st component */
    int g;       /* Alias for 2nd component */
    int b;       /* Alias for 3rd component */
    int a;       /* Alias for 4th component */
  };
  struct {
    int red;     /* Alias for 1st component */
    int green;   /* Alias for 2nd component */
    int blue;    /* Alias for 3rd component */
    int alpha;   /* Alias for 4th component */
  };
  struct {
    int vstart;  /* Alias for 1st component */
    int vcount;  /* Alias for 2nd component */
    int istart;  /* Alias for 3rd component */
    int icount;  /* Alias for 4th component */
  };
};

/* vec2 is a tuple of 2 floats
   Each field can be addressed using several aliases:
   - 1st component:  x, s
   - 2nd component:  y, t */
union __vec2 {
  float v[2];        /* All components at once */
  struct {
    float x;         /* Alias for 1st component */
    float y;         /* Alias for 2nd component */
  };
  struct {
    float s;         /* Alias for 1st component */
    float t;         /* Alias for 2nd component */
  };
  struct {
    float start;     /* Alias for 1st component */
    float end;       /* Alias for 2nd component */
  };
};

/* vec3 is a tuple of 3 floats
   Each field can be addressed using several aliases:
   - 1st component:  x, r, red
   - 2nd component:  y, g, green
   - 3rd component:  z, b, blue */
union __vec3 {
  float v[3];         /* All compoments at once */
  struct {
    float x;          /* Alias for 1st component */
    float y;          /* Alias for 2nd component */
    float z;          /* Alias for 3rd component */
  };
  struct {
    float r;          /* Alias for 1st component */
    float g;          /* Alias for 2nd component */
    float b;          /* Alias for 3rd component */
  };
  struct {
    float red;        /* Alias for 1st component */
    float green;      /* Alias for 2nd component */
    float blue;       /* Alias for 3rd component */
  };
  struct {
    union __vec2 v2;  /* Alias for the first 2 component */
    float zz;         /* the last component */
  };
};

/* vec4 is a tuple of 4 floats
   Each field can be addressed using several aliases:
   - 1st component:  x, r, red,   x_
   - 2nd component:  y, g, green, y_
   - 3rd component:  z, b, blue,  width
   - 4th component:  w, a, alpha, height */
union __vec4 {
  float v[4];         /* All compoments at once */
  struct {
    float x;          /* Alias for 1st component */
    float y;          /* Alias for 2nd component */
    float z;          /* Alias for 3rd component */
    float w;          /* Alias for 4th component */
  };
  struct {
    float x_;         /* Alias for 1st component */
    float y_;         /* Alias for 2nd component */
    float width;      /* Alias for 3rd component */
    float height;     /* Alias for 4th component */
  };
  struct {
    float r;          /* Alias for 1st component */
    float g;          /* Alias for 2nd component */
    float b;          /* Alias for 3rd component */
    float a;          /* Alias for 4th component */
  };
  struct {
    float red;        /* Alias for 1st component */
    float green;      /* Alias for 2nd component */
    float blue;       /* Alias for 3rd component */
    float alpha;      /* Alias for 4th component */
  };
  struct {
    float A;          /* Alias for 1st component */
    float B;          /* Alias for 2nd component */
    float C;          /* Alias for 3rd component */
    float D;          /* Alias for 4th component */
  };
  struct {
    union __vec3 v3;  /* Alias for the first 3 components */
    float ww;         /* the last component */
  };
};

/* mat2 stored like this:
   0 2
   1 3 */
union __mat2 {
  float m[4];    /* All compoments at once */
  struct {
    float m00, m01;   /* column 0 */
    float m10, m11;   /* column 1 */
  };
  struct {
    union __vec2 c0;  /* column 0 */
    union __vec2 c1;  /* column 1 */
  };
};

/* mat3 stored like this:
   0 3 6
   1 4 7
   2 5 8 */
union __mat3 {
  float m[9];    /* All compoments at once */
  struct {
    float m00, m01, m02;   /* column 0 */
    float m10, m11, m12;   /* column 1 */
    float m20, m21, m22;   /* column 2 */
  };
  struct {
    union __vec3 c0;       /* column 0 */
    union __vec3 c1;       /* column 1 */
    union __vec3 c2;       /* column 2 */
  };
};

/* mat4 stored like this:
   0 4  8 12
   1 5  9 13
   2 6 10 14
   3 7 11 15 */
union __mat4 {
  float m[16];   /* All compoments at once */
  struct {
    float m00, m01, m02, m03;   /* column 0 */
    float m10, m11, m12, m13;   /* column 1 */
    float m20, m21, m22, m23;   /* column 2 */
    float m30, m31, m32, m33;   /* column 3 */
  };
  struct {
    union __vec4 c0;            /* column 0 */
    union __vec4 c1;            /* column 1 */
    union __vec4 c2;            /* column 2 */
    union __vec4 c3;            /* column 3 */
  };
  struct {
    union __vec3 v0; float w0;  /* column 0 */
    union __vec3 v1; float w1;  /* column 1 */
    union __vec3 v2; float w2;  /* column 2 */
    union __vec3 v3; float w3;  /* column 3 */
  };
};


/*----------------------------- ivec2 functions -----------------------------*/
/* copy a 2d vector from another 2d vector */
static inline void ivec2_cpy( ivec2 *o, const ivec2 *i )
{
  o->x = i->x;
  o->y = i->y;
}

/* check if 2 ivec2s are equal */
int ivec2_eq( const ivec2 *i1, const ivec2 *i2 );



/*------------------------------ vec2 functions -----------------------------*/
/* Convenience constructor for vec2 structs */
static inline void vec2_set( vec2 *o, const float x, const float y )
{
  o->x = x;
  o->y = y;
}

/* copy a 2d vector from another 2d vector */
static inline void vec2_cpy( vec2 *o, const vec2 *i )
{
  o->x = i->x;
  o->y = i->y;
}

/* copy a 2d vector from a 3d vertor */
static inline void vec2_frm_vec3( vec2 *o, const vec3 *i )
{
  o->x = i->x;
  o->y = i->z;
}

/* Add two vectors */
static inline void vec2_add( vec2 *o, const vec2 *i1, const vec2 *i2 )
{
  o->x = i1->x + i2->x;
	o->y = i1->y + i2->y;
}

/* 2d vector self add */
static inline void vec2_self_add( vec2 *o, const vec2 *i )
{
  o->x += i->x;
  o->y += i->y;
}

/* Subtract two vectors */
static inline void vec2_sub( vec2 *o, const vec2 *i1, const vec2 *i2 )
{
  o->x = i1->x - i2->x;
  o->y = i1->y - i2->y;
}

/* sub a 2d vector from another 2d vector version 2*/
static inline void vec2_self_sub( vec2 *o, const vec2 *i )
{
  o->x -= i->x;
  o->y -= i->y;
}

/* multiply 2 2d vectors */
static inline void vec2_mul( vec2 *o, const vec2 *i1, const vec2 *i2 )
{
  o->x = i1->x*i2->x;
  o->y = i1->y*i2->y;
}

/* divid a 2d vector by another 2d vector */
static inline void vec2_div( vec2 *o, const vec2 *i1, const vec2 *i2 )
{
  o->x = i1->x/i2->x;
  o->y = i1->y/i2->y;
}

/* Negate a 2d vector */
static inline void vec2_neg( vec2 *o )
{
  o->x = -o->x;
	o->y = -o->y;
}

/* Scale a  2d vector */
static inline void vec2_scale( vec2 *o, const vec2 *i, const float s )
{
  o->x = i->x*s;
	o->y = i->y*s;
}

/* Scale a  2d vector */
static inline void vec2_self_scale( vec2 *o, const float s )
{
  o->x *= s;
	o->y *= s;
}

/* Vector dot product */
static inline float vec2_dot( const vec2 *i1, const vec2 *i2 )
{
  return i1->x*i2->x + i1->y*i2->y;
}

/* 2D vector cross product analog
   The cross product of 2D vectors results in a 3D vector with only a z component.
   This function returns the magnitude of the z value */
static inline float vec2_cross( const vec2 *i1, const vec2 *i2 )
{
  return i1->x*i2->y - i1->y*i2->x;
}

/* Returns the squared length of 2d vector */
static inline float vec2_lensq( const vec2 *i )
{
  return i->x*i->x + i->y*i->y;
}

/* Linearly interpolate between v1 and v2 */
static inline void vec2_lerp( vec2 *o, const vec2 *i1, const vec2 *i2, const float t )
{
  o->x = i1->x + (i2->x - i1->x)*t;
  o->y = i1->y + (i2->y - i1->y)*t;
}

/* implemented in t3d_vec2.c */
float vec2_len( const vec2 *i );
void vec2_norm( vec2 *o );
void vec2_take_min( vec2 *o, const vec2 *i );
void vec2_take_max( vec2 *o, const vec2 *i );


/*------------------------------ vec3 functions -----------------------------*/
/* set the values of vec3 structs */
static inline void vec3_set( vec3 *o, const float x, const float y, const float z )
{
  o->x = x;
  o->y = y;
  o->z = z;
}

/* copy a 3d vector from another 3d vector */
static inline void vec3_cpy( vec3 *o, const vec3 *i )
{
  o->x = i->x;
  o->y = i->y;
  o->z = i->z;
}

/* copy a 3d vector from a 4d vertor */
static inline void vec3_frm_vec4( vec3 *o, const vec4 *i )
{
  o->x = i->x;
  o->y = i->y;
  o->z = i->z;
}

/* 3d vector add */
static inline void vec3_add( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  o->x = i1->x + i2->x;
  o->y = i1->y + i2->y;
  o->z = i1->z + i2->z;
}

/* 3d vector self add */
static inline void vec3_self_add( vec3 *o, const vec3 *i )
{
  o->x += i->x;
  o->y += i->y;
  o->z += i->z;
}

/* 3d vector subtruct */
static inline void vec3_sub( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  o->x = i1->x - i2->x;
  o->y = i1->y - i2->y;
  o->z = i1->z - i2->z;
}

/* 3d vector self subtruct */
static inline void vec3_self_sub( vec3 *o, const vec3 *i )
{
  o->x -= i->x;
  o->y -= i->y;
  o->z -= i->z;
}

/* multiply 2 3d vectors */
static inline void vec3_mul( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  o->x = i1->x*i2->x;
  o->y = i1->y*i2->y;
  o->z = i1->z*i2->z;
}

/* divid a 3d vector by another 3d vector */
static inline void vec3_div( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  o->x = i1->x/i2->x;
  o->y = i1->y/i2->y;
  o->z = i1->z/i2->z;
}

/* negate a 3d vector */
static inline void vec3_neg( vec3 *o )
{
  o->x = -o->x;
  o->y = -o->y;
  o->z = -o->z;
}

/* scale a 3d vector */
static inline void vec3_scale( vec3 *o, const vec3 *i, const float s )
{
  o->x = i->x*s;
  o->y = i->y*s;
  o->z = i->z*s;
}

/* self scale a 3d vector */
static inline void vec3_self_scale( vec3 *o, const float s )
{
  o->x *= s;
  o->y *= s;
  o->z *= s;
}

/* dot product of 2 3d vectors */
static inline float vec3_dot( const vec3 *i1, const vec3 *i2 )
{
  return i1->x*i2->x + i1->y*i2->y + i1->z*i2->z;
}

/* cross product of 2 3d vectors, direction: i1 -> i2 ( right hand rule ) */
static inline void vec3_cross( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  o->x = i1->y*i2->z - i1->z*i2->y;
  o->y = i1->z*i2->x - i1->x*i2->z;
  o->z = i1->x*i2->y - i1->y*i2->x;
}

/* Returns the squared length of 3d vector */
static inline float vec3_lensq( const vec3 *i )
{
  return i->x*i->x + i->y*i->y + i->z*i->z;
}

/* linear interpolate 3d vectors */
static inline void vec3_lerp( vec3 *o, const vec3 *i1, const vec3 *i2, const float t )
{
  o->x = i1->x + (i2->x - i1->x)*t;
  o->y = i1->y + (i2->y - i1->y)*t;
  o->z = i1->z + (i2->z - i1->z)*t;
}

/* implemented in t3d_vec3.c */
float vec3_len( const vec3 *i );
void vec3_norm( vec3 *o );
void vec3_take_min( vec3 *o, const vec3 *i );
void vec3_take_max( vec3 *o, const vec3 *i );


/*------------------------------ vec4 functions -----------------------------*/
/* set the values of vec4 structs */
static inline void vec4_set( vec4 *o, const float x, const float y, const float z, const float w )
{
  o->x = x;
  o->y = y;
  o->z = z;
  o->w = w;
}

/* set the values of vec4 structs from a 3d vector and w element */
static inline void vec4_frm_v3w( vec4 *o, const vec3 *i, const float w )
{
  o->x = i->x;
  o->y = i->y;
  o->z = i->z;
  o->w = w;
}

/* set the values of vec4 structs from a 2d vector and z, w elements */
static inline void vec4_frm_v2zw( vec4 *o, const vec2 *i, const float z, const float w )
{
  o->x = i->x;
  o->y = i->y;
  o->z = z;
  o->w = w;
}

/* set the values of vec4 structs from 2 2d vectors */
static inline void vec4_frm_v2v2( vec4 *o, const vec2 *i1, const vec2 *i2 )
{
  o->x = i1->x;
  o->y = i1->y;
  o->z = i2->s;
  o->w = i2->t;
}

/* copy a 4d vector from another 4d vector */
static inline void vec4_cpy( vec4 *o, const vec4 *i )
{
  o->x = i->x;
  o->y = i->y;
  o->z = i->z;
  o->w = i->w;
}

/* negate a 4d vector */
static inline void vec4_neg( vec4 *o )
{
  o->x = -o->x;
  o->y = -o->y;
  o->z = -o->z;
  o->w = -o->w;
}

/* self scale a 4d vector */
static inline void vec4_self_scale( vec4 *o, const float s )
{
  o->x *= s;
  o->y *= s;
  o->z *= s;
  o->w *= s;
}

/* dot product of 2 4d vectors */
static inline float vec4_dot( const vec4 *i1, const vec4 *i2 )
{
  return i1->x*i2->x + i1->y*i2->y + i1->z*i2->z + i1->w*i2->w;
}

/* normalize a 4d vector */
void vec4_norm( vec4 *o );


/*--------------------------- quaternion functions --------------------------*/
/* a quaternion is also a 4d vector */
/* conjugate a quaternion */
static inline void quat_conjugate( vec4 *o, const vec4 *i )
{
  o->x = -i->x;
  o->y = -i->y;
  o->z = -i->z;
  o->w =  i->w;
}

/* normalize a quaternion */
void quat_norm( vec4 *o );
/* quaternion cross product, there is no 4d vector cross product */
void quat_cross( vec4 *o, const vec4 *i1, const vec4 *i2 );
/* multiply 2 quaterions */
void quat_mul( vec4 *o, const vec4 *i1, const vec4 *i2 );
/* set the quaternion from Axis & angle, angle in radians */
void quat_from_axis_angle( vec4 *o, const vec3 *axis, const float rad );
/* set quaternion by euler angles in radians */
void quat_from_eulers( vec4 *o, const float rad_x, const float rad_y, const float rad_z );
/* spherical linear interpolation between two quaternions */
void quat_slerp( vec4 *o, const vec4 *i1, const vec4 *i2, const float interp );


/*------------------------------ mat2 functions -----------------------------*/
/* Zero the mat2 */
static inline void mat2_zero( mat2 *o )
{
  memset(o, 0, sizeof(mat2));
}

static inline void mat2_identity( mat2 *o )
{
  memset(o, 0, sizeof(mat2));
  o->m00 = 1.0;
  o->m11 = 1.0;
}

/* note: entered in rows, but stored in columns */
static inline void mat2_set( mat2 *o,
                             const float a, const float b,
                             const float c, const float d )
{
#ifdef DEBUG
  /* assignment disorder, slow? */
  o->m[0] = a;   o->m[2] = b;
  o->m[1] = c;   o->m[3] = d;
#endif
  o->m[0] = a;
  o->m[1] = c;
  o->m[2] = b;
  o->m[3] = d;
}

/* copy a matrix from another matrix */
static inline void mat2_cpy( mat2 *o, const mat2 *i )
{
  /* memcpy might not be faster here? */
  o->m[0] = i->m[0];
  o->m[1] = i->m[1];
  o->m[2] = i->m[2];
  o->m[3] = i->m[3];
}

/* returns a scalar value with the determinant for mat2 */
static inline float mat2_determinant( const mat2 *i )
{
  return i->m00*i->m11 - i->m01*i->m10;
}

/* implemented in t3d_mat2.c */
/* set the matrix by 2 column vec2 vectors */
void mat2_v2set( mat2 *o, const vec2 *i0, const vec2 *i1 );
/* inverse a mat2 matrix */
void mat2_inv( mat2 *o, const mat2 *i );


/*------------------------------ mat3 functions -----------------------------*/
/* Zero the mat3 */
static inline void mat3_zero( mat3 *o )
{
  memset(o, 0, sizeof(mat3));
}

static inline void mat3_identity( mat3 *o )
{
  memset(o, 0, sizeof(mat3));
  o->m00 = 1.0;
  o->m11 = 1.0;
  o->m22 = 1.0;
}

/* implemented in t3d_mat3.c */
/* note: entered in rows, but stored in columns */
void mat3_set( mat3 *o,
               const float x0, const float y0, const float z0,
               const float x1, const float y1, const float z1,
               const float x2, const float y2, const float z2 );
/* set the matrix by 3 column vec3 vectors */
void mat3_v3set( mat3 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2 );
/* copy a matrix from another matrix */
void mat3_cpy( mat3 *o, const mat3 *i );
/* copy mat3 from upper left part of mat4 */
void mat3_frm_mat4( mat3 *o, const mat4 *i );
/* get a 9-element array flipped on the main diagonal */
void mat3_transpose( mat3 *o, const mat3 *i );
/* returns a scalar value with the determinant for mat3 */
float mat3_determinant( const mat3 *i );
/* inverse a mat3 matrix - general mathematical way */
void mat3_inv_math( mat3 *o, const mat3 *i );
/* inverse a mat3 matrix - optimized a little bit */
void mat3_inv( mat3 *o, const mat3 *i );


/*------------------------------ mat4 functions -----------------------------*/
/* Zero the mat4 */
static inline void mat4_zero( mat4 *o )
{
  memset(o, 0, sizeof(mat4));
}

static inline void mat4_identity( mat4 *o )
{
  memset(o, 0, sizeof(mat4));
  o->m00 = 1.0;
  o->m11 = 1.0;
  o->m22 = 1.0;
  o->m33 = 1.0;
}

/* implemented in t3d_mat4.c */
/* note: entered in rows, but stored in columns */
void mat4_set( mat4 *o,
               const float x0, const float y0, const float z0, const float w0,
               const float x1, const float y1, const float z1, const float w1,
               const float x2, const float y2, const float z2, const float w2,
               const float x3, const float y3, const float z3, const float w3 );
/* set the matrix by 4 column vec4 vectors */
void mat4_v4set( mat4 *o, const vec4 *i0, const vec4 *i1, const vec4 *i2, const vec4 *i3 );
/* set part of the matrix by 4 column vec3 vectors */
void mat4_v3set_3x4( mat4 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2, const vec3 *i3 );
/* set part of the matrix by 4 column vec3 vectors */
void mat4_v3set_3x3( mat4 *o, const vec3 *i0, const vec3 *i1, const vec3 *i2 );
/* copy a matrix from another matrix */
void mat4_cpy( mat4 *o, const mat4 *i );
/* get a row of a matrix, 3 elements */
void mat4_cpy_v3row( vec3 *o, const mat4 *i, const int pos );
/* get a row of a matrix, 4 elements */
void mat4_cpy_v4row( vec4 *o, const mat4 *i, const int pos );
/* get a column of a matrix, 3 elements */
void mat4_cpy_v3col( vec3 *o, const mat4 *i, const int pos );
/* get a column of a matrix, 4 elements */
void mat4_cpy_v4col( vec4 *o, const mat4 *i, const int pos );
/* add 2 matrices */
void mat4_add( mat4 *o, const mat4 *i1, const mat4 *i2 );
/* subtract one matrix from another matrix */
void mat4_sub( mat4 *o, const mat4 *i1, const mat4 *i2 );
/* prepare a scale matrix by scale factor */
void mat4_scale( mat4 *o, const mat4 *i, const float s );
/* prepare a scale matrix by scale factor */
void mat4_self_scale( mat4 *o, const float s );
/* multiply 2 matrices */
void mat4_mul( mat4 *o, const mat4 *i1, const mat4 *i2 );
/* get a 16-element array flipped on the main diagonal */
void mat4_transpose( mat4 *o, const mat4 *i );
/* returns a scalar value with the determinant for mat4 */
float mat4_determinant( const mat4 *i );
/* inverse a mat4 matrix - general mathematical way */
void mat4_inv_math( mat4 *o, const mat4 *i );
/* inverse a general mat4 matrix - using Cramer's Rule, optimized a bit */
void mat4_inv( mat4 *o, const mat4 *i );

/*------------------- Affine mat4 functions ---------------------------------*/
/* prepare a 4d matrix for translating with xyz array */
void mat4_set_trans( mat4 *o, const float x, const float y, const float z );
/* prepare a 4d matrix for rotating around x axis by an angle in radians */
void mat4_set_x_rot( mat4 *o, const float rad );
/* prepare a 4d matrix for rotating around y axis by an angle in radians */
void mat4_set_y_rot( mat4 *o, const float rad );
/* prepare a 4d matrix for rotating around z axis by an angle in radians */
void mat4_set_z_rot( mat4 *o, const float rad );
/* prepare a 4d matrix for rotate from euler angles, the angles are in radians */
void mat4_set_rot( mat4 *o, const float rad_x, const float rad_y, const float rad_z );
/* prepare a 4d matrix for rotating around an arbitrary axis by an angle in radians */
void mat4_set_axis_rot( mat4 *o, const vec3 *axis, const float rad );
/* compute the inverse of 4x4 Euclidean transformation matrix */
void mat4_inv_euclidean( mat4 *o, const mat4 *i );
/* compute the inverse of a 4x4 affine transformation matrix */
void mat4_inv_affine( mat4 *o, const mat4 *i );
/* prepare a orthographic matrix */
void mat4_set_ortho( mat4 *o, const float l, const float r, const float t, const float b, const float n, const float f );
/* prepare a frustum matrix */
void mat4_set_frustum( mat4 *o, const float l, const float r, const float t, const float b, const float n, const float f );
/* prepare a projection matrix */
void mat4_set_proj( mat4 *o, const float fovy, const float aspect, const float n, const float f );
/* prepare a view matrix */
void mat4_set_view( mat4 *o, const vec3 *p, const vec3 *f, const vec3 *u, const vec3 *r );
/* prepare a model matrix */
void mat4_set_model( mat4 *o, const vec3 *p, const vec3 *f, const vec3 *u, const vec3 *r );
/* convert quaternion to a matrix */
void mat4_from_quat( mat4 *o, const vec4 *i );
/* transform a vec4 by a mat4 */
void mat4_mul_vec4( vec4 *o, const mat4 *m, const vec4 *i );
/* rotate a 3d vector by rotation part of mat4 */
void mat4_r_mul_vec3( vec3 *o, const mat4 *m, const vec3 *i );
/* rotate by mat4's rotation part's transpose */
void mat4_inv_r_mul_vec3( vec3 *o, const mat4 *m, const vec3 *i );
/* translate a 3d vector by mat4's translation part */
void mat4_t_vec3( vec3 *o, const mat4 *m, const vec3 *i );
/* inverse translate a 3d vector by mat4's translation part */
void mat4_inv_t_vec3( vec3 *o, const mat4 *m, const vec3 *i );
/* rotate a 3d vector and then translate the 3d vector respectively */
void mat4_r_mul_t_vec3( vec3 *o, const mat4 *m, const vec3 *i );
/* inverse rotate a 3d vector and then translate the 3d vector respectively */
void mat4_inv_r_mul_t_vec3( vec3 *o, const mat4 *m, const vec3 *i );


#endif   /* _t3d_math_h_ */
