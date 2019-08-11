/*----- t3d_object.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_object_h_
#define _t3d_object_h_

#include <t3d_type.h>


enum {
  OBJ_NULL_ROT = 1,
  OBJ_ON_GROUND = 2,
  OBJ_U_ROT = 4,
  OBJ_R_ROT = 8,
  OBJ_F_ROT = 16,
  OBJ_FLY = 32,
  OBJ_ZOOM = 64
};

/*---- object properties ----------------------------------------------------+

  an object, such as, unit or camera, light has the following properties:

  pos            - oject position
  target         - target position the object towards

  forward        - forward direction - (target - pos)
  up             - Up direction
  right          - right (side) direction

  u_ref          - up reference vector (0.0, 1.0, 0.0)


  rot_speed      - degree per micro-second
	                 rotate about x-axis (pitch) or
                   rotate about y-axis (yaw) or
                   rotate about z-axis (roll)

  strafe_speed   - rotate about target point
                   rotate about x-axis of the target point or
                   rotate about y-axis of the target point or
                   rotate about z-axis of the target point

  move_speed     - move (translation) speed, per micro-second
                   forward or sideway

 +---------------------------------------------------------------------------*/


/* translate the object to a new postion */
void obj_move( vec3 *pos, vec3 *target, const vec3 *dir, const float dist, const int flag );
/* rotate the object by object's own position */
void obj_rot( vec3 *target, const vec3 *pos, const vec3 *axis, const float deg );
/* rotate the object about a given point(target) */
void obj_rot_by( vec3 *pos, const vec3 *target, const vec3 *axis, const float deg );
/* calculate the object's forward axis by objection position and target position */
void obj_find_f_axis( vec3 *forward, const vec3 *pos, const vec3 *target );
/*----------------------------------------------------------------------------+
  calculate directions using cross product of 2 perpendicular directions
  calculate the object's right axis, f->u = r    (vec3_cross(r, u, f))
  calculate the object's up axis, r->f = u       (vec3_cross(u, f, r))
  calculate the object's forward axis, u->r = u  (vec3_cross(f, r, u))
 +----------------------------------------------------------------------------*/
void obj_find_axis( vec3 *o, const vec3 *i1, const vec3 *i2 );


#endif  /* _t3d_object_h_ */