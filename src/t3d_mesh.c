/*----- t3d_mesh.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_mesh.h>


/* create a mesh struct in memory */
mesh *
mesh_new( const int n_vertices )
{
  mesh *msh = (mesh *)malloc(sizeof(mesh));

  msh->n_faces = n_vertices/3;
  msh->vcoords = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  msh->normals = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  msh->tangents = (vec3 *)malloc(sizeof(vec3)*n_vertices);
  msh->faces = (vec4 *)malloc(sizeof(vec4)*msh->n_faces);
  msh->hsr_cam_v_indices = (unsigned short *)malloc(sizeof(unsigned short)*n_vertices);
  msh->hsr_lit_v_indices = (unsigned short *)malloc(sizeof(unsigned short)*n_vertices);
  msh->cam_hsr_count = 0;
  msh->lit_hsr_count = 0;

  glGenBuffers(1, &msh->vbo_coords);
  glGenBuffers(1, &msh->vbo_normals);
  glGenBuffers(1, &msh->vbo_tangents);
  glGenBuffers(1, &msh->ibo_hsr_cam_elements);
  glGenBuffers(1, &msh->ibo_hsr_lit_elements);

  return msh;
}

/* delete a mesh struct form memory */
void
mesh_del( mesh *msh )
{
  if(!msh) return;

  glDeleteBuffers(1, &msh->vbo_coords);
  glDeleteBuffers(1, &msh->vbo_normals);
  glDeleteBuffers(1, &msh->vbo_tangents);
  glDeleteBuffers(1, &msh->ibo_hsr_cam_elements);
  glDeleteBuffers(1, &msh->ibo_hsr_lit_elements);
  free(msh->vcoords);
  free(msh->normals);
  free(msh->tangents);
  free(msh->faces);
  free(msh->hsr_cam_v_indices);
  free(msh->hsr_lit_v_indices);

  free(msh);
}