/*----- t3d_geomath.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_geomath_h_
#define _t3d_geomath_h_

#include <t3d_type.h>


enum {
  ON_PLANE = 0,
  PLANE_FRONT = 1,
  PLANE_BACK = 2
};


/* calculate the normal (point a->b->c in CCW order), not normalized */
void gmth_find_normal( vec3 *normal,
                       const vec3 *a,
                       const vec3 *b,
                       const vec3 *c );
/* calculate the plane dist part */
float gmth_plane_dist( const vec3 *normal,
                       const vec3 *point );
/* distance from plane to a 3d point */
float gmth_plane_point_dist( const vec4 *plane,
                             const vec3 *point );
/* create the plane with 3 points on the plane */
void gmth_plane_set_by_3points( vec4 *plane,
                                const vec3 *a,
                                const vec3 *b,
                                const vec3 *c );
/* return true if point is on the side that the normal points to or in the plane. */
int gmth_plane_classify_point( const vec4 *plane,
                               const vec3 *point );
/* checks if a line intersects a plane */
int gmth_is_line_x_plane( vec3 *normal, float *origin_distance,
                          const vec3 **poly,
                          const vec3 **line );
/* returns the intersection point of the line that intersects the plane */                                      
void gmth_find_x_point( vec3 *intersection,
                        const vec3 *normal, const float distance,
                        const vec3 **line );
/* return angle between 2 vec3 vectors */
float gmth_radians_from_v3v3( const vec3 *v1, const vec3 *v2 );
/* check if a point is inside the ranges of a polygon */
int gmth_is_in_polygon( const vec3 *point,
                        const vec3 **poly, const int vertex_count );
/* check if a line is intersecting a polygon */
int gmth_is_line_x_polygon( vec3 *intersection,
                            const vec3 **poly, const int vertex_count,
                            const vec3 **line );
/* compute the tangent of a triangle */
void gmth_find_tangent( vec3* tangent,
                        const vec3* p0, const vec3 *p1, const vec3 *p2,
                        const vec2* uv0, const vec2 *uv1, const vec2 *uv2 );


#endif   /* _t3d_geomath_h_ */
