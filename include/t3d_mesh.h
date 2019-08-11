/*----- t3d_mesh.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_mesh_h_
#define _t3d_mesh_h_

#include <t3d_type.h>


struct __mesh {
  vec3 *vcoords;                       /* vertices coordinates */
  vec3 *normals;                       /* vertex normals */
  vec3 *tangents;                      /* vertex tangents */
  vec4 *faces;                         /* triangle planes */
  unsigned short *hsr_cam_v_indices;   /* visible vertex indices to camera */
  unsigned short *hsr_lit_v_indices;   /* visible vertex indices to light */

  int cam_hsr_count;                   /* runtime count for cam hsr */
  int lit_hsr_count;                   /* runtime count for light hsr */

  int n_faces;
  /* vertex coordinates, normals, tangents have the same data size*/
  int v_data_size;                     /* num_vertices*sizeof(vec3) */

  unsigned int vbo_coords;             /* mesh specific - VBO vertex coordinates */
  unsigned int vbo_normals;            /* mesh specific - VBO vertex normals */
  unsigned int vbo_tangents;           /* mesh specific - VBO vertex tangents */
  unsigned int ibo_hsr_cam_elements;   /* mesh specific - IBO hsr camera element index */
  unsigned int ibo_hsr_lit_elements;   /* mesh specific - IBO hsr light element index */

  const unsigned int *tex_diffuse;
  const unsigned int *tex_normalmap;
  const unsigned int *tex_specular;
};


/* create a mesh struct in memory */
mesh *mesh_new( const int n_vertices );
/* delete a mesh struct form memory */
void mesh_del( mesh *msh );


#endif   /* _t3d_mesh_h_ */