/*----- t3d_patch.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_patch_h_
#define _t3d_patch_h_

#include <t3d_type.h>


/*---------------- calculate patch index buffer -----------------------------+

                    0---1---2---3---4                    ---
                    | / | / | / | / |                     |
	                  |/  |/  |/  |/  |                     |
	  degenerated --> 5---6---7---8---9 <-- degenerated  height = 3
    vertex (11)     | / | / | / | / |      vertex (8)     |
                    |/  |/  |/  |/  |                     |
                    10--11--12--13--14                   ---

                vertex (9, 10) degenerated

                    |-- width = 5 --|


  width  -- num_vertices_per_row                         [ex. w = 5]
  height -- num_vertices_per_col                         [ex. h = 3]

  num_vertices = w*h;                                    [ex. 5*3 = 15]
  num_strips_required = h - 1;                           [ex. 3-1 = 2]
  num_degens_required = 2*(num_strips_required - 1);     [ex. 2*(2-1) = 2]
  num_vertices_per_strip = 2*w;                          [ex. 2*5 = 10]

  num_indices = num_vertices_per_strip*num_strips_required +
                num_degens_required;                     [ex. 10*2+2 = 22]

 +----------------------------------------------------------------------------*/

/* calculate patch index buffer */
void patch_v_indexing( unsigned int *indices, const int width, const int height );
/* calculate the texture coordinates of a patch */
void patch_find_texcoords( vec2 *texcoords, const int width, const int height );


#endif   /* _t3d_patch_h_ */