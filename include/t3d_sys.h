/*----- t3d_system.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_system_h_
#define _t3d_system_h_

#include <t3d_type.h>


enum {
  CLR_RED = 0,
  CLR_GREEN = 1,
  CLR_BLUE = 2,
  CLR_WHITE = 3,
  CLR_BLACK = 4,
  CLR_YELLOW = 5,
  CLR_CYAN = 6,
  CLR_PINK = 7
};


struct __t3dsys {
  list *units;                    /* a list of units */
  ms3d **models;                  /* ms3d modes */
  unsigned int n_models;

  hashtable *texdb;               /* texture ( name, id ) database */
  hashtable *modeldb;             /* model ( name, id ) information database */

  patchmap *pchmap;               /* patchmap */

  float time_passed;
  vec2 timer;                     /* system timer (start --- end) */

  vec4 colors[8];                 /* some color constants */
};


/* create a new system struct in memory */
t3dsys *sys_new( void );
/* delete ogl from memory */
void sys_del( t3dsys *sys );


#endif   /* _t3d_system_h_ */
