/*----- t3d_shader.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_shader_h_
#define _t3d_shader_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


struct __shader {
  /*----------------------- glsl programs -----------------------------------*/
  GLuint prog_normal_pass;
  GLuint prog_light_pass;
  GLuint prog_font;
  GLuint prog_box2d;
  GLuint prog_aamesh;
  GLuint prog_minimap;

  /*----------------------- vertex shader -----------------------------------*/
  GLint attri_v_coord;            /* attributes - vertex coordinates */
  GLint attri_v_normal;           /* attributes - vertex normals */
  GLint attri_v_texcoord;         /* attributes - vertex texture coordinates */
  GLint attri_v_tangent;          /* attributes - vertex tangents */

  GLint uniform_vp;               /* uniform - proj*view matrix */
  GLint uniform_v;                /* uniform - view matrix */
  GLint uniform_m;                /* uniform - model matrix */
  GLint uniform_m3x3_inv_transp;  /* uniform - inverse transposed model matrix */

  GLint uniform_lvp;              /* uniform - light proj*view matrix */
  GLint uniform_bias_lvp;         /* uniform - light bias*proj*view matrix */

  GLint uniform_mvp;              /* uniform - proj*view*model matrix */

  GLint uniform_tex_offset;       /* uniform - texture coordinate offset for aamesh */

  /*----------------------- fragment shader ----------------------------------*/
  GLint uniform_v_inv;            /* uniform - inversed view matrix */

  GLint uniform_diffmap;          /* uniform - diffuse */
  GLint uniform_normmap;          /* uniform - normalmap */
  GLint uniform_specmap;          /* uniform - specular */
  GLint uniform_shadmap;          /* uniform - shadowmap */

  GLint uniform_light_coord;      /* uniform - light postion */
  GLint uniform_onecolor;         /* uniform - onecolor */
};


/* create a shader object in memory */
shader *shd_new( void );
/* delete a shader from memory */
void shd_del( shader *s );
/* load the vertex and fragment shaders */
GLuint shd_load( const char *vert_file, const char *frag_file );
/* map to glsl attributes, uniforms for normalmap shader */
void shd_set( shader *s, const GLuint program );



#endif   /* _t3d_shader_h_ */
