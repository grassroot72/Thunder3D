/*----- t3d_patch.c ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <t3d_math.h>


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

  num_strips_required = h - 1;                           [ex. 3-1 = 2]
  num_degens_required = 2*(num_strips_required - 1);     [ex. 2*(2-1) = 2]
  num_vertices_per_strip = 2*w;                          [ex. 2*5 = 10]

  num_indices = num_vertices_per_strip*num_strips_required +
                num_degens_required;                     [ex. 10*2+2 = 22]

 +----------------------------------------------------------------------------*/
void
patch_v_indexing( unsigned int *indices, const int width, const int height )
{
  int base, height_;
  int x, y, count = 0;
  unsigned int index;

  height_ = height - 1;

  for(y = 0; y < height_; y++) {
    base = y*width;
    for(x = 0; x < width; x++) {
      index = base + x;
      indices[count] = index;
      count++;

      index = base + width + x;
      indices[count] = index;
      count++;
    }
    /* add a degenerate triangle (except in the last row) */
    if(y < height_ - 1) {
      index = (y + 1)*width + (width - 1);
      indices[count] = index;
      count++;

      index = (y + 1)*width;
      indices[count] = index;
      count++;
    }
  }
}

/* calculate the texture coordinates of a patch */
void
patch_find_texcoords( vec2 *texcoords, const int width, const int height )
{
  int x, z, count = 0;

  for(z = 0; z < height; z++) {
    for(x = 0; x < width; x++) {
      texcoords[count].s = x/(float)(width - 1);
      texcoords[count].t = z/(float)(height - 1);
      count++;
    }
  }
}