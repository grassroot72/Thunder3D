/*----- t3d_ms3d.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_ms3d_h_
#define _t3d_ms3d_h_

#include <t3d_type.h>


typedef struct __ms3djoint ms3djoint;

struct __ms3dskin {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  float shininess;      /* 0.0f - 128.0f */
  float transparency;   /* 0.0f - 1.0f */
  char texture[32];
  char alphamap[32];
};

struct __ms3d {
  ms3dmesh **o_mshs;            /* ms3d meshes */
  ms3dskin *o_skins;            /* ms3d materials */
  ms3djoint *o_joints;          /* base - original joints */
  unsigned short n_mshs;        /* number of meshes */
  unsigned short n_skins;       /* number of materials */
  unsigned short n_joints;      /* number of Joints */

  float anim_fps;               /* animation FPS */
  float frametime;              /* frame time */
  float total_time;             /* total animation frame time */
  unsigned short n_cmds;
  ms3dcmd *anim_cmds;           /* total animation commands */

  vec3 aabb_min;                /* AABBox minimums */
  vec3 aabb_max;                /* AABBox maximums */
  float radius;                 /* AABBox diagonal length */

  decal *o_aamsh;               /* the mesh that the model is sitting on */
};

struct __ms3dcmd {
  unsigned short start, end;
  char cmd;
};

struct __ms3danim {              /* <---- Used by unit */
  float cur_time;                /* current frame time in an animation */

  char cur_cmd;                  /* current animation command */
  vec2 ani_time;                 /* time vector of an animatioin (start --- end) */
  unsigned short *cur_transkeys; /* for animation */
  unsigned short *cur_rotkeys;   /* for animation */

  int islooping;
};


/* load static ms3d model vertices */
ms3d *ms3d_new( const unsigned char *buf, const hashtable *texdb );
/* delete ms3d from memory */
void ms3d_del( ms3d *m );
/* create the ms3d animation struct in memory */
ms3danim *ms3d_anim_new( const ms3d *model );
/* delete the ms3d animationf struct from memory */
void ms3d_anim_del( ms3danim *ani );
/* restart the animation */
void ms3d_reset_anim( ms3danim *ani, const unsigned short n_joints );
/* Set animation command */
void ms3d_set_anim( ms3danim *ani, const ms3d *m, const char cmd );
/* calculate the animation time */
void ms3d_calc_anim_time( ms3danim *ani, const ms3d *m, const float time_passed );
/* load animation commands from a vector containing string(command) lines */
ms3dcmd *ms3d_animcmd_new( unsigned short *n_cmds, const vector *strlines );
/* animate the ms3d model, aim to calculate the mat_finals */
void ms3d_animate( mat4 *mat_finals, ms3danim *ani, const ms3d *m );


#endif   /* _t3d_ms3d_h_ */