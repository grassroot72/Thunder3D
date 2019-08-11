/*----- t3d_shader.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_util.h>
#include <t3d_math.h>
#include <t3d_shader.h>


/* create a shader object in memory */
shader *
shd_new( void )
{
  shader *s = (shader *)malloc(sizeof(shader));

  s->attri_v_coord = 0;
  s->attri_v_normal = 1;
  s->attri_v_tangent = 2;
  s->attri_v_texcoord = 3;

  return s;
}

/* delete a shader from memory */
void
shd_del( shader *s )
{
  glDeleteProgram(s->prog_normal_pass);
  glDeleteProgram(s->prog_light_pass);
  glDeleteProgram(s->prog_font);
  glDeleteProgram(s->prog_box2d);
  glDeleteProgram(s->prog_aamesh);
  glDeleteProgram(s->prog_minimap);
  free(s);
}

/* compile shader source and generate a handle */
static GLuint
shd_compile( const char* source, const GLenum type )
{
  GLint compile_status;
  GLuint handle = glCreateShader(type);
  glShaderSource(handle, 1, &source, 0);
  glCompileShader(handle);

  glGetShaderiv(handle, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == GL_FALSE) {
    GLchar messages[256];
    glGetShaderInfoLog(handle, sizeof(messages), 0, &messages[0]);
    fprintf(stderr, "%s\n", messages);
    exit(EXIT_FAILURE);
  }

  return handle;
}

/* load the vertex and fragment shaders */
GLuint
shd_load( const char *vert_file, const char *frag_file )
{
  GLuint handle = glCreateProgram();
  GLint link_status;

  if(vert_file && strlen(vert_file)) {
    char *vert_src = util_file_to_mem(vert_file);
    GLuint vert_shader = shd_compile(vert_src, GL_VERTEX_SHADER);
    glAttachShader(handle, vert_shader);
    free(vert_src);
  }
  if(frag_file && strlen(frag_file)) {
    char *frag_src = util_file_to_mem(frag_file);
    GLuint frag_shader = shd_compile(frag_src, GL_FRAGMENT_SHADER);
    glAttachShader(handle, frag_shader);
    free(frag_src);
  }

  glBindAttribLocation(handle, 0, "v_coord");
  glBindAttribLocation(handle, 1, "v_normal");
  glBindAttribLocation(handle, 2, "v_tangent");
  glBindAttribLocation(handle, 3, "v_texcoord");

  glLinkProgram(handle);
  glGetProgramiv(handle, GL_LINK_STATUS, &link_status);
  if(link_status == GL_FALSE) {
    GLchar messages[256];
    glGetProgramInfoLog(handle, sizeof(messages), 0, &messages[0]);
    fprintf(stderr, "%s\n", messages);
    exit(EXIT_FAILURE);
  }

  return handle;
}

/* map to glsl attributes, uniforms for normalmap shader */
void
shd_set( shader *s, const GLuint program )
{
  /*----------------------- vertex shader -----------------------------------*/
  s->uniform_vp = glGetUniformLocation(program, "vp");
  //s->uniform_v = glGetUniformLocation(program, "v");
  s->uniform_m = glGetUniformLocation(program, "m");
  s->uniform_m3x3_inv_transp = glGetUniformLocation(program, "m3x3_inv_transp");

  s->uniform_lvp = glGetUniformLocation(program, "lvp");
  s->uniform_bias_lvp = glGetUniformLocation(program, "bias_lvp");

  s->uniform_mvp = glGetUniformLocation(program, "mvp");

  s->uniform_tex_offset = glGetUniformLocation(program, "tex_offset");

  /*----------------------- fragment shader ----------------------------------*/
  s->uniform_v_inv = glGetUniformLocation(program, "v_inv");

  s->uniform_diffmap = glGetUniformLocation(program, "diffmap");
  s->uniform_normmap = glGetUniformLocation(program, "normmap");
  s->uniform_specmap = glGetUniformLocation(program, "specmap");
  s->uniform_shadmap = glGetUniformLocation(program, "shadmap");

  s->uniform_light_coord = glGetUniformLocation(program, "light_pos");
  s->uniform_onecolor = glGetUniformLocation(program, "onecolor");
}