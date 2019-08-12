/*----- t3d_astar.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_astar_h_
#define _t3d_astar_h_

#include <t3d_type.h>


enum {
  ASTAR_AVAIL = 1,
  ASTAR_UNAVAIL = 0
};


struct __anode {
  int x;
	int y;

  int f;
  int g;
  int h;

  struct __anode *next;
  struct __anode *father;
};


/* calculates a pathfind from start to end cells using A* algorithm */
int astar_search( ivec2 **path, const ivec2 *start, const ivec2 *end, const patchmap *pm );


#endif   /* _t3d_astar_h_ */