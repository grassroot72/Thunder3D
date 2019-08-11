/*----- t3d_patchmap.h -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_patchmap_h_
#define _t3d_patchmap_h_

#include <pthread.h>
#include <t3d_type.h>


/*---------------- calculate patch index buffer -----------------------------+

       ex. patch w = 3, h = 3;

       |--w=3--|

  ---  0---1---2---3---4---5---6---7---8---9---10--11--12--13--14--15--16  ---
   |   | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / |    |
   |   |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |    |
  h=3  17--18--19--20--21--22--23--24--25--26--27--28--29--30--31--32--33   |
   |   | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / |    |
   |   |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |    |
  ---  34--35--36--37--38--39--40--41--42--43--44--45--46--47--48--49--50  H=5
       | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / |    |
       |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |    |
  ---  51--52--53--54--55--56--57--58--59--60--61--62--63--64--65--66--67   |
   |   | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / | / |    |
   |   |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |/  |    |
  ---  68--69--70--71--72--73--74--75--76--77--78--79--80--81--82--83--84  ---

       |---| <-- cell width

       |--------------------------W=17---------------------------------|

       ex. Heightmap W = 17, H = 5


  num_total_vertices = W*H;                            [ex. 17*5 = 85]

  num_cells_per_row = W - 1;                           [ex. 17-1 = 16]
  num_cells_per_col = H - 1;                           [ex. 5-1 = 4]
  num_total_cells = num_cells_per_row*
                    num_cells_per_col;                 [ex. 16*4 = 64]

  num_cells_per_patch_row = w - 1;                     [ex. 3-1 = 2]
  num_cells_per_patch_col = h - 1;                     [ex. 3-1 = 2]
  num_total_patch_cells = num_cells_per_patch_row*
                          num_cells_per_patch_col;     [ex. 2*2 = 4]

  num_patch_vertices = w*h;                            [ex. 3*3 = 9]
  num_patches_per_row = num_cells_per_row/
                        num_cells_per_patch_row;       [ex. 16/2 = 8]
  num_patches_per_col = num_cells_per_col/
                        num_cells_per_patch_col;       [ex. 4/2 = 2]
  num_total_patches = num_patches_per_row*
                      num_patches_per_col;             [ex. 8*2 = 16]


 +----------------------------------------------------------------------------*/


struct __cell {
  vec4 planes[2];
  vec3 center;                /* the center of the cell, for A* path finding */

  int status;                 /* for A* path finding - availabe or not */
};

struct __patchmap {
  vec3 *vcoords;               /* attribute - vertex coordinates of the patchmap */
  vec3 *normals;               /* attribute - vertex normals of the patchmap */
  vec3 *tangents;              /* attribute - vertex tangents of the patchmap */

  vec2 *pch_texcoords;         /* attribute - vertex texture coordinates in a patch */
  unsigned int *pch_indices;   /* indices in a patch */

  cell *cells;                 /* patchmap cells - 2 triangles per cell */
  geopatch **pchs;             /* geopatches */

  pthread_mutex_t lock;        /* mutex lock to lock patchmap cells */

  float step;                  /* step size between 2 vertices */
  float scale;                 /* the scale in y direction */
  int w;                       /* heightmap width, in vertices */
  int h;                       /* heightmap height, in vertices */
  int n_total_vertices;        /* total number of vertices in the heightmap */

  int w_cells;                 /* number of cells per row */
  int h_cells;                 /* number of cells per column */
  int n_total_cells;           /* total number of cells in the heightmap */

  int w_pch_cells;             /* number of cells per patch row */
  int h_pch_cells;             /* number of cells per patch column */
  int n_total_pch_cells;       /* total number of cells in a patch */

  int pch_w;                   /* must be 2^n + 1, in vertices */
  int pch_h;                   /* must be 2^n + 1, in vertices */
  int n_total_pch_vertices;    /* total number of vertices in a patch */        

  int w_pchs;                  /* number of patches per row */
  int h_pchs;                  /* humber of patches per column */
  int n_total_pchs;            /* total number of patches */

  int n_total_pch_indices;     /* total number of indices in a patch */

  unsigned int vbo_texcoords;  /* VBO - vertex texture coordinates */
  unsigned int ibo_elements;   /* IBO - element (triangle strip) index */

  mat4 mat_model;              /* model matrix */
  mat3 mat_m3x3;               /* model rotation 3x3 matrix */
};


/* creata a patchmap struct in memory */
patchmap *pchmap_new( const char *file,
                      const int patch_w,
                      const int patch_h,
                      const float htmap_step,
                      const float htmap_scale,
                      const hashtable *texdb );
/* delete a patchmap struct from memory */
void pchmap_del( patchmap *pm );
/* update patchmap visibility against frustum 
void pchmap_update_visi( patchmap *pm, const camera *cam, const light *lit );*/
/* draw the patchmap with normalmap */
void pchmap_normalmap_draw( const patchmap *pm, const light *lit, const shader *glsl );
/* draw the patchmap with one vertex coordinates */
void pchmap_vertex_draw( const patchmap *pm, const shader *glsl );


#endif   /* _t3d_patchmap_h_ */
