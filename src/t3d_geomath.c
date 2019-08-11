/*----- t3d_geomath.c -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <math.h>
#include <t3d_math.h>
#include <t3d_geomath.h>


/* calculate the normal (point a->b->c in CCW order), not normalized */
void
gmth_find_normal( vec3 *normal,
                  const vec3 *a,
                  const vec3 *b,
                  const vec3 *c )
{
  vec3 a2b, a2c;

  /* Get 2 vectors from the polygon (2 sides), remember the order! */
  vec3_sub(&a2b, b, a);
  vec3_sub(&a2c, c, a);
  /* take the cross product of our 2 vectors to get a perpendicular vector */
  vec3_cross(normal, &a2b, &a2c);
  /* now we have a normal, but it's at a strange length */
  vec3_norm(normal);
}

/* calculate the plane dist part */
float
gmth_plane_dist( const vec3 *normal,
                 const vec3 *point )
{
  /* this variable holds the distance from the plane to the origin */
  float distance = 0.0;

  /* use the plane equation to find the distance ( Ax + By + Cz + D = 0 )
     we want to find D.
     so, we come up with D = -( Ax + By + Cz ) */

  /* the negated dot product of the normal of the plane and the point */
  distance = -vec3_dot(normal, point);

  return distance;
}

/* distance from plane to a 3d point */
float
gmth_plane_point_dist( const vec4 *plane,
                       const vec3 *point )
{
  return plane->A*point->x + plane->B*point->y + plane->C*point->z + plane->D;
}

/* create the plane with 3 points on the plane */
void
gmth_plane_set_by_3points( vec4 *plane,
                           const vec3 *a,
                           const vec3 *b,
                           const vec3 *c )
{
  vec3 normal;

  gmth_find_normal(&normal, a, b, c);
  vec4_frm_v3w(plane, &normal, -vec3_dot(&normal, a));
}

/* return true if point is on the side that the normal points to or in the plane. */
int
gmth_plane_classify_point( const vec4 *plane,
                           const vec3 *point )
{
  float dist = gmth_plane_point_dist(plane, point);

  if(dist > EPSILON)
    return PLANE_FRONT;

  if(dist < -EPSILON)
    return PLANE_BACK;

  return ON_PLANE;
}

/* checks if a line intersects a plane */
int
gmth_is_line_x_plane( vec3 *normal, float *origin_distance,
                      const vec3 **poly,
                      const vec3 **line )
{
  /* the distances from the 2 points of the line from the plane */
  float distance1 = 0.0;
  float distance2 = 0.0;

  /* we need to get the normal of our plane to go any further */
  gmth_find_normal(normal, poly[0], poly[1], poly[2]);
  /* let's find the distance our plane is from the origin.
     we can find this value from the normal to the plane (polygon)
     and any point that lies on that plane (any vertex) */
  *origin_distance = gmth_plane_dist(normal, poly[0]);

  /* get the distance from point1 from the plane using:
     Ax + By + Cz + D = (The distance from the plane)
     distance = (normal->x*line[0]->x  +                      Ax + 
                 normal->y*line[0]->y  +                      Bx +
                 normal->z*line[0]->z) + *origin_distance;    Cz + D */

  /* get the distance from point1 from the plane */
  distance1 = vec3_dot(normal, line[0]) + *origin_distance;
  /* get the distance from point2 from the plane */
  distance2 = vec3_dot(normal, line[1]) + *origin_distance;

  /* now that we have 2 distances from the plane, if we times them together
     we either get a positive or negative number.

     if it's a negative number, that means we collided!
     because the 2 points must be on either side of the plane
     (IE. -1 * 1 = -1).*/

  /* Check if both point's distances are both negative or both positive */
  if(distance1*distance2 >= 0.0)
    /* return FALSE if each point has the same sign.
       -1 and 1 would mean each point is on either side of the plane,
       3 4 wouldn't... */
    return 0;
                    
  return 1;     /* the line intersected the plane, return TRUE */
}

/* returns the intersection point of the line that intersects the plane */                                      
void
gmth_find_x_point( vec3 *intersection,
                   const vec3 *normal, const float distance,
                   const vec3 **line )
{
  /* variables to hold the point and the line's direction */
  vec3 scaled_line;
  vec3 line_dir;
  float numerator = 0.0;
  float denominator = 0.0;
  float dist = 0.0;

  /* (1) first we need to get the vector of our line, Then normalize it so
         it's a length of 1 */
  vec3_sub(&line_dir, line[1], line[0]);   /* Get the Vector of the line */
  vec3_norm(&line_dir);                    /* Normalize the lines vector */

  /* (2) use the plane equation ( distance = Ax + By + Cz + D ) to find the 
         distance from one of our points on the line to the plane */
  /* use the plane equation with the normal and the line
  numerator = -(normal->x*line0->x +
                normal->y*line0->y +
                normal->z*line0->z + distance); */
  numerator = -(vec3_dot(normal, line[0]) + distance);

  /* (3) if we take the dot product between our line vector and the normal
         of the polygon */
  /* get the dot product of the line's vector and the normal of the plane */
  denominator = vec3_dot(normal, &line_dir);

  /* since we are using division, we need to make sure we don't get a divide
     by zero error.

     if we do get a 0, that means that there are INFINATE points because the
     the line is on the plane (the normal is perpendicular to the line),
     in this case, return any point on the line */

  if(denominator < EPSILON)   /* check so we don't divide by zero */
    /* return an arbitrary point on the line */
    vec3_cpy(intersection, line[0]);

  /* divide to get the multiplying (percentage) factor */
  dist = numerator/denominator;

  /* now, as we said above, we times the dist by the vector, then add
     our arbitrary point */
  vec3_scale(&scaled_line, &line_dir, dist);
  vec3_add(intersection, line[0], &scaled_line);
}

/* return angle between 2 vec3 vectors */
float
gmth_radians_from_v3v3( const vec3 *v1, const vec3 *v2 )
{         
  float dot_product;
  float vectors_magnitude;
  float val;
  float radians;

  /* get the dot product of the vectors */
  dot_product = vec3_dot(v1, v2);              

  /* get the product of both of the vectors magnitudes */
  vectors_magnitude = vec3_len(v1)*vec3_len(v2);

  /* get the angle in radians between the 2 vectors */
  /* if(vectors_magnitude == 0.0) return 0.0; */
  val = dot_product/vectors_magnitude;
  if(val < -1.0)  val = -1.0;
  if(val >  1.0)  val =  1.0;

  radians = acosf(val);

  /* Here we make sure that the angle is not a -1.#IND0000000 number,
     which means indefinate */
  /* if( IsNan(radians))  return 0.0; */

  /* return the angle in radians */
  return radians;
}

/* check if a point is inside the ranges of a polygon */
int
gmth_is_in_polygon( const vec3 *point,
                    const vec3 **poly, const int vertex_count )
{
  int i;
  /* used to cover up the error in floating point */
  const float MATCH_FACTOR = 0.99999;
  float radians = 0.0;    /* initialize the angle */
  vec3 a, b;              /* temp vectors */

  /* go in a circle to each vertex and get the angle between */
  for( i = 0; i < vertex_count; i++) {
    /* subtract the intersection point from the current vertex */
    vec3_sub(&a, poly[i], point);
    /* Subtract the point from the next vertex */
    vec3_sub(&b, poly[(i + 1)%vertex_count], point);
    /* find the angle between the 2 vectors and add them all up */
    radians += gmth_radians_from_v3v3(&a, &b);
  }

  /* if the angle is greater than 2 PI, (360 degrees) */
  if(radians >= (MATCH_FACTOR*(2.0*M_PI)))
    return 1;     /* The point is inside of the polygon */

  /* it wasn't inside the polygon, so return FALSE */
  return 0;
}

/* check if a line is intersecting a polygon
   if return TRUE, then intersection->y is the height */
int
gmth_is_line_x_polygon( vec3 *intersection,
                        const vec3 **poly, const int vertex_count,
                        const vec3 **line )
{
  vec3 normal;
  float origin_distance = 0.0;

  /* (1) first, make sure our line intersects the plane */
                                   /* Ref */         /* Ref */
  if(!gmth_is_line_x_plane(&normal, &origin_distance, poly, line))
    return 0;

  /* (2) now that we have our normal and distance passed back from
         geo_line_x_plane(), we can alculate the intersection point */
  gmth_find_x_point(intersection, &normal, origin_distance, line);

  /* (3) now that we have the intersection point, check if it's
         inside the polygon */
  if(gmth_is_in_polygon(intersection, poly, vertex_count))
    return 1;          /* we collided! return TRUE */
    
  return 0;            /* no collision, return FALSE */
}

/* compute the tangent of a triangle */
void
gmth_find_tangent( vec3* tangent,
                   const vec3* p0, const vec3 *p1, const vec3 *p2,
                   const vec2* uv0, const vec2 *uv1, const vec2 *uv2 )
{
  vec3 side0, side1;
  vec3_sub(&side0, p1, p0);
  vec3_sub(&side1, p2, p0);

  vec2 uv_side0, uv_side1;
  vec2_sub(&uv_side0, uv1, uv0);
  vec2_sub(&uv_side1, uv2, uv0);

  float c = uv_side0.s*uv_side1.t - uv_side1.s*uv_side0.t;
  if(c != 0) {
    float s = 1.0/c;
    /* tangent = (side0*uv_side1.t - side1*uv_side0.t)*s; */
    tangent->x = (side0.x*uv_side1.t - side1.x*uv_side0.t)*s;
    tangent->y = (side0.y*uv_side1.t - side1.y*uv_side0.t)*s;
    tangent->z = (side0.z*uv_side1.t - side1.z*uv_side0.t)*s;
  }
  else
    vec3_set(tangent, 0.0, 0.0, 0.0);
}