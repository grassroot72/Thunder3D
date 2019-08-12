/*----- t3d_unit.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_unit_h_
#define _t3d_unit_h_

#include <t3d_type.h>


struct __unit {
  ms3d *model;                  /* pointer to model */

  mesh **mshs;                  /* meshes */
  ms3danim *ani;                /* ms3d animation */
  mat4 *mat_joint_finals;       /* finalized matrices of model joints, for animation */

  int color;                    /* unit color index, friend/enemy/NPC */

  int visible;                  /* if visible to camera */
  int picked;                   /* if picked by mouse */

  vec3 center_ws;               /* need to be transformed by mat_model */
  vec3 center;                  /* AABBox center */
  float half_x_len;             /* AABBox half size in x direction */
  float half_y_len;             /* AABBox half size in y direction */
  float half_z_len;             /* AABBox half size in z direction */
//vec3 aabb_min;                /* dyanmic AABBox minimums */
//vec3 aabb_max;                /* dyanmic AABBox maximums */

  aamesh *aamsh;                /* landing axis aligned decal mesh */

  float move_speed;             /* movement speed */
  float rot_speed;              /* rotation speed */
  float strafe_speed;           /* strafing speed */

  vec3 pos;                     /* unit's position */
  vec3 target;                  /* unit's target position */

  vec3 f;                       /* unit's forward direction */
  vec3 u;                       /* unit's up direction */
  vec3 r;                       /* unit's right direction */

  ivec2 ipos;                   /* unit's 2d position on heightmap */
  ivec2 itarget;                /* 2d target cell */

  ivec2 *path;                  /* found path */
  int chk_path;                 /* if calculate the path or not */
  int n_steps;                  /* number of steps in the path */
  vec3 last_waypoint;           /* the last waypoint of the path */
  vec3 target_f;                /* target direction */

  mat4 mat_model;               /* model matrix */
  mat3 mat_m3x3;                /* model rotation 3x3 matrix */
};


/* create unit struct in memory */
unit *unit_new( ms3d **models, const unsigned int model_id );
/* destroy unit struct */
void unit_del( unit *u );
/* set unit's position */
void unit_set_pos( unit *u, const vec3 *pos, patchmap *pm );
/* set the unit's direction */
void unit_set_dir( unit *u, const vec3 *forward );
/* set unit skin set */
void unit_set_skin( unit *u, const int skin_index, const int skin_stride, const hashtable *texdb );
/* update unit model vertices */
void unit_update_vertices( unit *u );
/* calculate the unit triangle faces */
void unit_calc_faces( unit *u );
/* calculate HSR for camera */
void unit_calc_cam_hsr( unit *u, const vec3 *object_space_unit_to_cam, const int flag );
/* calculate HSR for light */
void unit_calc_lit_hsr( unit *u, const vec3 *object_space_unit_to_lit, const int flag );
/* update VBO */
void unit_update_vbo( const unit *u );
/* update the camera hsr ibo */
void unit_update_hsr_cam_ibo( const unit *u );
/* update the light hsr ibo */
void unit_update_hsr_lit_ibo( const unit *u );
/* draw the unit model with normalmap */
void unit_normalmap_draw( const unit *u, const light *lit, const shader *glsl );
/* draw the unit model with only vertex coordinates */
void unit_vertex_draw( const unit *u, const shader *glsl );


#endif  /* _t3d_unit_h_ */