/*----- t3d_font.c -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_util.h>
#include <t3d_texture.h>
#include <t3d_shader.h>
#include <t3d_camera.h>
#include <t3d_font.h>


struct __bakedchar {
  unsigned short x0, y0, x1, y1; /*coordinates of bbox in bitmap */
  float xoff, yoff, xadvance;   
};

/* store vertices in graphic card buffers */
static void
fnt_upload( const font *f )
{
  glBindBuffer(GL_ARRAY_BUFFER, f->vbo_coord);
  /* 4 vertices (vec4 = vec2vec2) of a box, check fnt_print */
  glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*4, NULL, GL_DYNAMIC_DRAW);
}

/* create font struct in memory */
font *
fnt_new( const char *fontname,
         const float size,
         const int tex_w,
         const int tex_h,
         const int first_char,
         const int count,
         const int screen_w,
         const int screen_h )
{
  font *f = (font *)malloc(sizeof(font));

  f->cdata = (bakedchar *)malloc(sizeof(bakedchar)*count);
  f->size = size;
  f->tex_w = tex_w;
  f->tex_h = tex_h;
  f->first_char = first_char;
  f->count = count;

  unsigned char *buf = (unsigned char *)util_file_to_mem(fontname);
  unsigned char *texels = (unsigned char *)malloc(sizeof(unsigned char)*tex_w*tex_h);
  stbtt_BakeFontBitmap(buf, 0, size,
                       texels, tex_w, tex_h,
                       first_char, count,
                       (stbtt_bakedchar *)f->cdata);
  free(buf);

  f->tex_id = tex_gen_red_tex(texels, tex_w, tex_h);
  tex_set_flags(f->tex_id, TEX_USE_LINEAR|TEX_TO_BORDER);

  free(texels);

  /* create the vertex buffer object for font */
  glGenBuffers(1, &f->vbo_coord);
  fnt_upload(f);

  mat4_set_ortho(&f->mat_proj, 0.0, (float)screen_w, 0.0, (float)screen_h, 0.0, 10.0);

  return f;
}

/* delete font struct from memory */
void
fnt_del( font *f )
{
  if(!f)  return;

  glDeleteBuffers(1, &f->vbo_coord);
  glDeleteTextures(1, &f->tex_id);
  free(f->cdata);
  free(f);
}

/* print the text */
void
fnt_print( font *f, const wchar_t *text,
           float x, float y,
           const vec4 *color,
           shader *glsl )
{
  glUseProgram(glsl->prog_font);
  shd_set(glsl, glsl->prog_font);

  glUniformMatrix4fv(glsl->uniform_mvp, 1, GL_FALSE, f->mat_proj.m);
  glUniform1i(glsl->uniform_diffmap, /*GL_TEXTURE*/0);
  glUniform4f(glsl->uniform_onecolor, color->r, color->g, color->b, color->a);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, f->tex_id);

  glEnableVertexAttribArray(glsl->attri_v_coord);
  glBindBuffer(GL_ARRAY_BUFFER, f->vbo_coord);

  /* Loop through all characters */
  const wchar_t *p;
	for(p = text; *p; p++) {
    stbtt_aligned_quad quad;
    bakedchar *bchar = f->cdata + (*p - f->first_char);

    int round_x = (int)floor(x + bchar->xoff);
    int round_y = (int)floor(y - bchar->yoff);

    quad.x0 = (float)round_x;
    quad.y0 = (float)round_y;
    quad.x1 = (float)round_x + bchar->x1 - bchar->x0;
    quad.y1 = (float)round_y - bchar->y1 + bchar->y0;

    quad.s0 = bchar->x0/(float)f->tex_w;
    quad.t0 = bchar->y0/(float)f->tex_w;
    quad.s1 = bchar->x1/(float)f->tex_h;
    quad.t1 = bchar->y1/(float)f->tex_h;
			
    x += bchar->xadvance;

    void *f_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    vec4 *box = (vec4 *)f_data;

    box[0].x = quad.x1;
    box[0].y = quad.y0;
    box[0].z = quad.s1;
    box[0].w = quad.t0;

    box[1].x = quad.x0;
    box[1].y = quad.y0;
    box[1].z = quad.s0;
    box[1].w = quad.t0;

    box[2].x = quad.x1;
    box[2].y = quad.y1;
    box[2].z = quad.s1;
    box[2].w = quad.t1;

    box[3].x = quad.x0;
    box[3].y = quad.y1;
    box[3].z = quad.s0;
    box[3].w = quad.t1;

    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(glsl->attri_v_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  glDisableVertexAttribArray(glsl->attri_v_coord);
}