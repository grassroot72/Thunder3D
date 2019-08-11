/*----- T3D_texture.h --------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_texture_h_
#define _t3d_texture_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


enum {
  TEX_ST3C_COMPRESS = 1,
  TEX_USE_LINEAR = 2,
  TEX_USE_NEAREST = 4,
  TEX_USE_MIPMAPS = 8,
  TEX_REPEATS = 16,
  TEX_TO_EDGE = 32,
  TEX_TO_BORDER = 64
};


/* set texture flags */
void tex_set_flags( const GLuint tex_id, const GLuint flags );
/* Loads an image from disk into an OpenGL texture */
GLuint tex_load( const char *file, const int s3tc_compressed );
/* generate an empty GL_RED texture */
GLuint tex_gen_red_tex( const unsigned char *texels, const int tex_w, const int tex_h );


#endif	/* _t3d_texture_h_ */