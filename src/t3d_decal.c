/*----- t3d_decal.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_vector.h>
#include <t3d_patch.h>
#include <t3d_decal.h>


/* store texture coordinates, elements in graphic card buffers */
static void
decal_upload( const decal *d )
{
  glBindBuffer(GL_ARRAY_BUFFER, d->vbo_o_texcoords);
  glBufferData(GL_ARRAY_BUFFER,
               d->n_total_vertices*sizeof(vec2),
               d->o_texcoords,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->ibo_o_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               d->n_total_indices*sizeof(unsigned int),
               d->o_indices,
               GL_STATIC_DRAW);
}

/* create a decal target mesh in memory */
decal *
decal_new( const float radius, const float htmap_step )
{
  decal *d = (decal *)malloc(sizeof(decal));

  /* +1 because we are in the middle of the target mesh */
  d->half_w_cells = (int)(radius/htmap_step);
	d->w_cells = d->half_w_cells*2 + 1;
  d->w = d->w_cells + 1;

  d->n_total_vertices = d->w*d->w;

  int n_strips_required = d->w_cells;
  int n_degens_required = 2*(n_strips_required - 1);
  int n_vertices_per_strip = 2*d->w;
  d->n_total_indices = n_vertices_per_strip*n_strips_required +
                       n_degens_required;

  d->o_texcoords = (vec2 *)malloc(sizeof(vec2)*d->n_total_vertices);
  d->o_indices = (unsigned int *)malloc(sizeof(unsigned int)*d->n_total_indices);

  /* find the texture coordinates of a decal patch */
  patch_find_texcoords(d->o_texcoords, d->w, d->w);
  /* indexing the decal patch */
  patch_v_indexing(d->o_indices, d->w, d->w);

  /* create the vertex buffer object for decal mesh */
  glGenBuffers(1, &d->vbo_o_texcoords);
  glGenBuffers(1, &d->ibo_o_elements);

  decal_upload(d);

  return d;
}

/* delete a decal from memory */
void
decal_del( decal *d )
{
  if(!d) return;

  glDeleteBuffers(1, &d->vbo_o_texcoords);
  glDeleteBuffers(1, &d->ibo_o_elements);
  free(d->o_indices);
  free(d->o_texcoords);
  free(d);
}