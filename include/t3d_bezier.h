/*----- t3d_bezier.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_bezier_h_
#define _t3d_bezier_h_

#include <t3d_type.h>


/* get a list of bezier points */
vec2 *bezier_get_points( const vec2 *points, const int n_points, const float t );



#endif   /* _t3d_bezier_h_ */