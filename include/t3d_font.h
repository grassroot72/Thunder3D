/*----- t3d_font.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_font_h_
#define _t3d_font_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


typedef struct __bakedchar bakedchar;

/* structure to create a TTF font texture in video memory */
struct __font {
  char name[256];               /* internal name to use for the font */

  bakedchar *cdata;             /* the TTF font character data, contains UVs and offsets */
  float size;                   /* the font size used at creation time */

  int tex_w;                    /* recommended to use a value that is a power of 2 */
  int tex_h;                    /* recommended to use a value that is a power of 2 */

  int first_char;               /* the first character (in ASCII value) to use as the start character */
  int count;                    /* how many ASCII should be generated from the first_char ASCII */

  GLuint tex_id;

  GLuint vbo_coord;             /* VBO - font vertex coordinate + texture coordinate */

  mat4 mat_proj;                /* projection (otho) matrix */
};


/* create font struct in memory */
font *fnt_new( const char *fontname,
               const float size,
               const int tex_w,
               const int tex_h,
               const int first_char,
               const int count,
               const int screen_w,
               const int screen_h );
/* delete font struct from memory */
void fnt_del( font *f );
/* print the text */
void fnt_print( font *f, const wchar_t *text,
                float x, float y,
                const vec4 *color,
                shader *glsl );


#endif  /* _t3d_font_h_ */