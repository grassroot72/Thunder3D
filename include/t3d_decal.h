/*----- t3d_decal.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_decal_h_
#define _t3d_decal_h_

#include <t3d_type.h>


struct __decal {
  vec2 *o_texcoords;             /* texture coordinates */
  unsigned int *o_indices;       /* vertex indices of the target mesh */

  int half_w_cells;
  int w_cells;                   /* number of cells per decal row */
  int w;                         /* width - number of vertice per decal row */
  int n_total_vertices;
  int n_total_indices;

  GLuint vbo_o_texcoords;
  GLuint ibo_o_elements;
};


/* create a decal target mesh in memory */
decal *decal_new( const float radius, const float htmap_step_size );
/* delete a decal from memory */
void decal_del( decal *d );


#endif   /* _t3d_decal_h_ */