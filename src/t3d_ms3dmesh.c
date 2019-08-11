/*----- t3d_ms3dmesh.c ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_ms3dmesh.h>


/* create a ms3dmesh struct in memory */
ms3dmesh *
ms3dmesh_new( const int n_vertices )
{
  ms3dmesh *o_msh = (ms3dmesh *)malloc(sizeof(ms3dmesh));

  o_msh->o_vcoords = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  o_msh->o_normals = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  o_msh->o_tangents = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  o_msh->o_texcoords = (vec2 *)malloc(sizeof(vec2)*n_vertices);
  o_msh->o_jnt_indices = (short *)malloc(sizeof(short)*n_vertices);
  o_msh->n_vertices = n_vertices;

  glGenBuffers(1, &o_msh->vbo_o_texcoords);

  return o_msh;
}

/* delete a ms3dmesh struct form memory */
void
ms3dmesh_del( ms3dmesh *o_msh )
{
  if(!o_msh) return;

  glDeleteBuffers(1, &o_msh->vbo_o_texcoords);
  free(o_msh->o_vcoords);
  free(o_msh->o_normals);
  free(o_msh->o_tangents);
  free(o_msh->o_texcoords);
  free(o_msh->o_jnt_indices);

  free(o_msh);
}