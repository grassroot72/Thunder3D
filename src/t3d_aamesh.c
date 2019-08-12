/*----- t3d_aamesh.c --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_vector.h>
#include <t3d_hashtable.h>
#include <t3d_texture.h>
#include <t3d_heightmap.h>
#include <t3d_patchmap.h>
#include <t3d_camera.h>
#include <t3d_decal.h>
#include <t3d_aamesh.h>


/* store vertices in graphic card buffers */
static void
aamesh_upload( const aamesh *aamsh )
{
  glBindBuffer(GL_ARRAY_BUFFER, aamsh->vbo_coord);
  glBufferData(GL_ARRAY_BUFFER, aamsh->v_data_size, NULL, GL_DYNAMIC_DRAW);
}

/* create a axis aligned decal target mesh in memory */
aamesh *
aamesh_new( const decal *d )
{
  aamesh *aamsh = (aamesh *)malloc(sizeof(aamesh));

  aamsh->v_data_size = d->n_total_vertices*sizeof(vec3);
  aamsh->vcoords = (vec3 *)malloc(aamsh->v_data_size);

  /* create the vertex buffer object for decal mesh */
  glGenBuffers(1, &aamsh->vbo_coord);
  aamesh_upload(aamsh);

  mat4_identity(&aamsh->mat_model);

  return aamsh;
}

/* delete an axis aligned decal from memory */
void
aamesh_del( aamesh *aamsh )
{
  if(!aamsh) return;

  glDeleteBuffers(1, &aamsh->vbo_coord);
  free(aamsh->vcoords);
  free(aamsh);
}

/* set the axis aligned decal texture */
void
aamesh_set_tex( aamesh *aamsh, const hashtable *texdb )
{
  aamsh->tex_id = hash_get(texdb, "circle.jpg");
  tex_set_flags(*aamsh->tex_id, TEX_USE_MIPMAPS|TEX_TO_BORDER);
}

/* find patchmap vertices of the axis aligned decal patch */
void
aamesh_get_mesh( aamesh *aamsh,
                 const decal *d,
                 const int x,       /* unit's 2d position x on heightmap */
                 const int y,       /* unit's 2d position y on heightmap */
                 const patchmap *pm )
{
  int start_x = x - d->half_w_cells;
  int start_y = y - d->half_w_cells;
  int end_x = start_x + d->w;
  int end_y = start_y + d->w;

  int i, j, count = 0;
  for(j = start_y; j < end_y; j++) {
    for(i = start_x; i < end_x; i++) {
      unsigned int v_index = pm->w*j + i;
      memcpy(&aamsh->vcoords[count], &pm->vcoords[v_index], sizeof(vec3));
      count++;
    }
  }
}

/* find aamesh texture offset */
void
aamesh_tex_offset( aamesh *aamsh,
                   const decal *d,
                   const float x,    /* unit's 2d postion x on patchmap */
                   const float y,    /* unit's 2d postion y on patchmap */
                   const float htmap_step )
{
  float mod_x = fmodf(x, htmap_step);
  float mod_y = fmodf(y, htmap_step);

  aamsh->tex_offset.x = (0.5*htmap_step - mod_x)/((float)d->w_cells*htmap_step);
  aamsh->tex_offset.y = (0.5*htmap_step - mod_y)/((float)d->w_cells*htmap_step);
}

/* update vbo */
void
aamesh_update_vbo( const aamesh *aamsh )
{
  glBindBuffer(GL_ARRAY_BUFFER, aamsh->vbo_coord);
  void *aa_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  memcpy(aa_data, aamsh->vcoords, aamsh->v_data_size);
  glUnmapBuffer(GL_ARRAY_BUFFER);
}


/* draw the aamesh */
void
aamesh_draw( aamesh *aamsh,
             const decal *d,
             const camera *cam,
             const vec4 *color,
             shader *glsl )
{
  glUniformMatrix4fv(glsl->uniform_vp, 1, GL_FALSE, cam->mat_vp.m);
  glUniformMatrix4fv(glsl->uniform_m, 1, GL_FALSE, aamsh->mat_model.m);
  glUniform2f(glsl->uniform_tex_offset, aamsh->tex_offset.x, aamsh->tex_offset.y);
  glUniform1i(glsl->uniform_diffmap, /*GL_TEXTURE*/0);
  glUniform4f(glsl->uniform_onecolor, color->r, color->g, color->b, color->a);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *aamsh->tex_id);

  glEnableVertexAttribArray(glsl->attri_v_coord);
  glEnableVertexAttribArray(glsl->attri_v_texcoord);

  glBindBuffer(GL_ARRAY_BUFFER, aamsh->vbo_coord);
  glVertexAttribPointer(glsl->attri_v_coord,   /* attribute */
                        3,                     /* (x, y, z) */
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        0);

  glBindBuffer(GL_ARRAY_BUFFER, d->vbo_o_texcoords);
  glVertexAttribPointer(glsl->attri_v_texcoord,  /* attribute */
                        2,                       /* (x, y) */
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        0);

  /* push each element to the vertex shader */
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d->ibo_o_elements);
  glDrawElements(GL_TRIANGLE_STRIP, d->n_total_indices, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(glsl->attri_v_texcoord);
  glDisableVertexAttribArray(glsl->attri_v_coord);
}
