/*----- t3d_quat.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


/* normalize a quaternion */
void
quat_norm( vec4 *o )
{
  float len = sqrtf(o->x*o->x + o->y*o->y + o->z*o->z + o->w*o->w);
  if(len < EPSILON) return;

  float ilen = 1.0/len;
  o->x *= ilen;
  o->y *= ilen;
  o->z *= ilen;
  /*o->w *= ilen;*/
}

/* quaternion cross product, there is no 4d vector cross product */
void
quat_cross( vec4 *o, const vec4 *i1, const vec4 *i2 )
{
  o->x = i1->w*i2->x + i1->x*i2->w + i1->y*i2->z - i1->z*i2->y;
  o->y = i1->w*i2->y + i1->x*i2->z + i1->y*i2->w - i1->z*i2->x;
  o->z = i1->w*i2->z + i1->x*i2->y + i1->y*i2->x - i1->z*i2->w;
  o->w = i1->w*i2->w - i1->x*i2->x - i1->y*i2->y - i1->z*i2->z;
}

/* multiply 2 quaterions */
void
quat_mul( vec4 *o, const vec4 *i1, const vec4 *i2 )
{
  o->x = i1->w*i2->x + i1->x*i2->w + i1->y*i2->z - i1->z*i2->y;
  o->y = i1->w*i2->y - i1->x*i2->z + i1->y*i2->w + i1->z*i2->x;
  o->z = i1->w*i2->z + i1->x*i2->y - i1->y*i2->x + i1->z*i2->w;
  o->w = i1->w*i2->w - i1->x*i2->x - i1->y*i2->y - i1->z*i2->z;
}

/* set the quaternion from axis & angle, angle in radians */
void
quat_from_axis_angle( vec4 *o, const vec3 *axis, const float rad )
{
  float sa, ca;

  sa = sinf(rad*0.5);
  ca = cosf(rad*0.5);
  o->x = axis->x*sa;
  o->y = axis->y*sa;
  o->z = axis->z*sa;
  o->w = ca;
}

/* set quaternion by euler angles in radians */
void
quat_from_eulers( vec4 *o, const float rad_x, const float rad_y, const float rad_z  )
{
  float sr, sp, sy, cr, cp, cy;
  float crcp, srsp;
  float rad;

  rad = rad_x*0.5;
  sr = sinf(rad);         /* sr - sin roll */
  cr = cosf(rad);         /* cr - cos roll */
  rad = rad_y*0.5;
  sp = sinf(rad);         /* sp - sin pitch */
  cp = cosf(rad);         /* cp - cos pitch */
  rad = rad_z*0.5;
  sy = sinf(rad);         /* sy - sin yaw */
  cy = cosf(rad);         /* cy - cos yaw */

  crcp = cr*cp;
  srsp = sr*sp;

  o->x = sr*cp*cy - cr*sp*sy;
  o->y = cr*sp*cy + sr*cp*sy;
  o->z = crcp*sy - srsp*cy;
  o->w = crcp*cy + srsp*sy;
}

/* spherical linear interpolation between two quaternions */
void
quat_slerp( vec4 *o, const vec4 *i1, const vec4 *i2, const float interp )
{
  float cos_theta = 0.0;
  float sin_theta = 0.0;
  float beta = 0.0;
  vec4 q2_array;
  float t;

  /* temporary array to hold second quaternion */
  t = interp;
  vec4_cpy(&q2_array, i2);

  cos_theta = vec4_dot(i1, i2);

  if(cos_theta < 0.0) {
    vec4_neg(&q2_array);  /* flip sigh if so */
    cos_theta = -cos_theta;
  }

  beta = 1.0 - t;

  if(1.0 - cos_theta > EPSILON) {
    /* we are using spherical interpolation */
    cos_theta = acosf(cos_theta);
    sin_theta = 1.0/sinf(cos_theta);
    beta = sinf(cos_theta*beta)*sin_theta;
    t = sinf(cos_theta*t)*sin_theta;
  }

  /* Interpolation. */
  o->x = beta*i1->x + t*q2_array.x;
  o->y = beta*i1->y + t*q2_array.y;
  o->z = beta*i1->z + t*q2_array.z;
  o->w = beta*i1->w + t*q2_array.w;
}