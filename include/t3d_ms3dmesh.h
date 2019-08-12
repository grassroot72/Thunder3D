/*----- t3d_ms3dmesh.h -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_ms3dmesh_h_
#define _t3d_ms3dmesh_h_

#include <t3d_type.h>


struct __ms3dmesh {
  vec3 *o_vcoords;               /* original vertices */
  vec3 *o_normals;               /* original vertex normals */
  vec3 *o_tangents;              /* original vertex tangents */
  vec2 *o_texcoords;             /* original texture coordinates */
  short *o_jnt_indices;          /* original joint indices */

  int n_vertices;

  unsigned int vbo_o_texcoords;  /* mesh specific - VBO vertex texture coordinates */

  char material_index;           /* switch among the materials in ms3d model */
};


/* create a ms3dmesh struct in memory */
ms3dmesh *ms3dmesh_new( const int n_vertices );
/* delete a ms3dmesh struct form memory */
void ms3dmesh_del( ms3dmesh *o_msh );


#endif   /* _t3d_ms3dmesh_h_ */
