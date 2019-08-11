/*----- t3d_geopatch.c ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_vector.h>
#include <t3d_geopatch.h>


/* create a geopatch struct in memory */
geopatch *
geopch_new( void )
{
  geopatch *pch = (geopatch *)malloc(sizeof(geopatch));

  glGenBuffers(1, &pch->vbo_coords);
  glGenBuffers(1, &pch->vbo_normals);
  glGenBuffers(1, &pch->vbo_tangents);

  return pch;
}

/* delete a geopatch struct form memory */
void
geopch_del( geopatch *pch )
{
  if(!pch) return;

  glDeleteBuffers(1, &pch->vbo_coords);
  glDeleteBuffers(1, &pch->vbo_normals);
  glDeleteBuffers(1, &pch->vbo_tangents);

  free(pch);
}

/* store vertices, normals tangents in graphic card buffers */
void
geopch_upload( const geopatch *pch,
               const vec3 *vcoords,
               const vec3 *normals,
               const vec3 *tangents,
               const unsigned int v_data_size )
{
  glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_coords);
  glBufferData(GL_ARRAY_BUFFER, v_data_size, vcoords, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_normals);
  glBufferData(GL_ARRAY_BUFFER, v_data_size, normals, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_tangents);
  glBufferData(GL_ARRAY_BUFFER, v_data_size, tangents, GL_STATIC_DRAW);
}