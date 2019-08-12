/*----- t3d_object.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <t3d_math.h>
#include <t3d_object.h>


/* translate the object to a new postion */
void
obj_move( vec3 *pos, vec3 *target, const vec3 *dir, const float dist, const int flag )
{
  if(flag & OBJ_ON_GROUND) {
    /*-----------------------------------------------------------------------+
      Don't do Y to keep object on the ground
     +-----------------------------------------------------------------------*/
    pos->x += dir->x*dist;
    pos->z += dir->z*dist;

    /* Move along with the position */
    target->x += dir->x*dist;
    target->z += dir->z*dist;
  }
  else if(flag & OBJ_FLY) {
    pos->x += dir->x*dist;
    pos->y += dir->y*dist;
    pos->z += dir->z*dist;

    /* Move along with the position */
    target->x += dir->x*dist;
    target->y += dir->y*dist;
    target->z += dir->z*dist;
  }
  else {    /* closing to target(lookat point), zooming */
    pos->x += dir->x*dist;
    pos->y += dir->y*dist;
    pos->z += dir->z*dist;
  }
}

/* calculate the object rotation, return a quaternion */
static void
obj_calc_rot( vec4 *q, const vec3 *axis, const float deg )
{
  float rad;
  /* convert from degree to radians */
  rad = DEG2RAD(deg);

  /* Create the rotation quaternion based on the axis we are rotating on */
  quat_from_axis_angle(q, axis, rad);
  quat_norm(q);
}

static void
obj_calc_quat_pos( vec4 *q_pos, const vec3 *to, const vec3 *from, const vec4 *q_rot )
{
  vec4 q_to, q_tmp1, q_tmp2;

  /*-------------------------------------------------------------------------+
    Create the view quaternion.
    This will be the direction from 'from' to 'to'.
   +-------------------------------------------------------------------------*/
  q_to.x = to->x - from->x;
  q_to.y = to->y - from->y;
  q_to.z = to->z - from->z;
  q_to.w = 0.0;

  /*-------------------------------------------------------------------------+
    Create the resulting quaternion by multiplying the rotation quat
    by the view quat, then multiplying that by the conjugate of
    the rotation quat
   +-------------------------------------------------------------------------*/
  quat_mul(&q_tmp1, q_rot, &q_to);
  quat_conjugate(&q_tmp2, q_rot);
  quat_mul(q_pos, &q_tmp1, &q_tmp2);
}

/* rotate the object by object's own position */
void
obj_rot( vec3 *target, const vec3 *pos, const vec3 *axis, const float deg )
{
  vec4 q_rot, q_new_target;

  obj_calc_rot(&q_rot, axis, deg);
  obj_calc_quat_pos(&q_new_target, target, pos, &q_rot);

  /*-------------------------------------------------------------------------+
    Update the view information by adding the position to
    the resulting quaternion
   +-------------------------------------------------------------------------*/
  target->x = pos->x + q_new_target.x;
  target->y = pos->y + q_new_target.y;
  target->z = pos->z + q_new_target.z;
}

/* rotate the object about a given point(target) */
void
obj_rot_by( vec3 *pos, const vec3 *target, const vec3 *axis, const float deg )
{
  vec4 q_rot, q_new_pos;

  obj_calc_rot(&q_rot, axis, deg);
  obj_calc_quat_pos(&q_new_pos, pos, target, &q_rot);
  /*-------------------------------------------------------------------------+
    Update the position information by adding the position to
    the resulting quaternion
   +-------------------------------------------------------------------------*/
  pos->x = target->x + q_new_pos.x;
  pos->y = target->y + q_new_pos.y;
  pos->z = target->z + q_new_pos.z;
}

/* calculate forward axis with objection position and target position */
void
obj_find_f_axis( vec3 *forward, const vec3 *pos, const vec3 *target )
{
  /* calculate the forward axis */
  vec3_sub(forward, target, pos);
  vec3_norm(forward);
}

/*----------------------------------------------------------------------------+
  calculate directions using cross product of 2 perpendicular directions
  calculate the object's right axis, f->u = r    (vec3_cross(r, u, f))
  calculate the object's up axis, r->f = u       (vec3_cross(u, f, r))
  calculate the object's forward axis, u->r = u  (vec3_cross(f, r, u))
 +----------------------------------------------------------------------------*/
void
obj_find_axis( vec3 *o, const vec3 *i1, const vec3 *i2 )
{
  /* calulate the right axis */
  vec3_cross(o, i1, i2);
  vec3_norm(o);
}