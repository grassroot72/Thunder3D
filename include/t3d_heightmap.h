/*----- t3d_heightmap.h -----------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_heightmap_h_
#define _t3d_heightmap_h_

#include <t3d_type.h>


/* load heightmap from file */
unsigned char *htmap_from_file( const char *file, int *w, int *h, int *n_vertices );

/* fill vertex position and texture coordinates */
void htmap_load_vertices( vec3 *coords,
                          vec3 *normals,
                          vec3 *tangents,
                          vec2 *texcoords,
                          const unsigned char *raw_heights,
                          const int htmap_w,
                          const int htmap_h,
                          const float htmap_step,
                          const float htmap_scale );

/* calculate tangents, normals and cell planes of the heightmap */
void htmap_find_tangents( vec3 *tangents,
                          vec3 *normals,
                          cell *cells,
                          const vec3 *vcoords,
                          const vec2 *texcoords,
                          const int n_total_vertices,
                          const int htmap_w,
                          const int htmap_h,
                          const int w_cells,
                          const int h_cells );


#endif   /* _t3d_heightmap_h_ */