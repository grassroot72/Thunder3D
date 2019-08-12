/*----- t3d_minimap.c -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_texture.h>
#include <t3d_hashtable.h>
#include <t3d_minimap.h>


/* store vertices in graphic card buffers */
static void
minimap_upload( const minimap *mm )
{
  glBindBuffer(GL_ARRAY_BUFFER, mm->vbo_coord);
  /* 4 vertices (vec4 = vec2vec2) of a box, check minimap_draw */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*4, NULL, GL_DYNAMIC_DRAW);
}

/* create minimap struct in memory */
minimap *
minimap_new( const char *mapname,
             const int screen_w,
             const int screen_h,
             const hashtable *texdb )
{
  minimap *mm = (minimap *)malloc(sizeof(minimap));

  mm->tex_id = hash_get(texdb, mapname);
  tex_set_flags(*mm->tex_id, TEX_USE_LINEAR|TEX_TO_BORDER);

  /* create the vertex buffer object for font */
  glGenBuffers(1, &mm->vbo_coord);
  minimap_upload(mm);

  mat4_set_ortho(&mm->mat_proj, 0.0, (float)screen_w, 0.0, (float)screen_h, 0.0, 10.0);

  return mm;
}

/* delete minimap struct from memory */
void
minimap_del( minimap *mm )
{
  if(!mm)  return;

  glDeleteBuffers(1, &mm->vbo_coord);
  free(mm);
}

/* draw the minimap */
void
minimap_draw( minimap *mm, shader *glsl )
{
  glUseProgram(glsl->prog_minimap);
  shd_set(glsl, glsl->prog_minimap);

  glUniformMatrix4fv(glsl->uniform_mvp, 1, GL_FALSE, mm->mat_proj.m);
  glUniform1i(glsl->uniform_diffmap, /*GL_TEXTURE*/0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, *mm->tex_id);

  glEnableVertexAttribArray(glsl->attri_v_coord);
  glBindBuffer(GL_ARRAY_BUFFER, mm->vbo_coord);

  void *mm_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  vec4 *box = (vec4 *)mm_data;

  box[0].x = 8.0;
  box[0].y = 8.0;
  box[0].z = 0.0;
  box[0].w = 0.0;

  box[1].x = 8.0;
  box[1].y = 188.0;
  box[1].z = 1.0;
  box[1].w = 0.0;

  box[2].x = 188.0;
  box[2].y = 8.0;
  box[2].z = 0.0;
  box[2].w = 1.0;

  box[3].x = 188.0;
  box[3].y = 188.0;
  box[3].z = 1.0;
  box[3].w = 1.0;

  glUnmapBuffer(GL_ARRAY_BUFFER);

  glVertexAttribPointer(glsl->attri_v_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glDisableVertexAttribArray(glsl->attri_v_coord);
}