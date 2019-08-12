/*----- t3d_ogl.c ------------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <GL/glfw3.h>
#include <t3d_ogl.h>

/* macro to resolve OpenGL function pointers */
#define GET_GL_FN(type, var, name)                \
        if(status == GL_TRUE) {                   \
          var = (type)glfwGetProcAddress((name)); \
          if((var) == NULL) {                     \
            status = GL_FALSE;                    \
          }                                       \
        }


/* resolve the opengl functions */
GLboolean
ogl_init( void )
{
  GLboolean status = GL_TRUE;

  GET_GL_FN(PFNGLBLENDFUNCPROC, glBlendFunc, "glBlendFunc");
  GET_GL_FN(PFNGLCLEARPROC, glClear, "glClear");
  GET_GL_FN(PFNGLCLEARCOLORPROC, glClearColor, "glClearColor");
  GET_GL_FN(PFNGLCOLORMASKPROC, glColorMask, "glColorMask");
  GET_GL_FN(PFNGLCULLFACEPROC, glCullFace, "glCullFace");
  GET_GL_FN(PFNGLDEPTHFUNCPROC, glDepthFunc, "glDepthFunc");
  GET_GL_FN(PFNGLDEPTHMASKPROC, glDepthMask, "glDepthMask");
  GET_GL_FN(PFNGLDISABLEPROC, glDisable, "glDisable");
  GET_GL_FN(PFNGLDRAWARRAYSPROC, glDrawArrays, "glDrawArrays");
  GET_GL_FN(PFNGLDRAWELEMENTSPROC, glDrawElements, "glDrawElements");
  GET_GL_FN(PFNGLENABLEPROC, glEnable, "glEnable");
  GET_GL_FN(PFNGLGETFLOATVPROC, glGetFloatv, "glGetFloatv");
  GET_GL_FN(PFNGLPOLYGONMODEPROC, glPolygonMode, "glPolygonMode");
  GET_GL_FN(PFNGLPOLYGONOFFSETPROC, glPolygonOffset, "glPolygonOffset");
  GET_GL_FN(PFNGLVIEWPORTPROC, glViewport, "glViewport");
  GET_GL_FN(PFNGLACTIVETEXTUREPROC, glActiveTexture, "glActiveTexture");
  GET_GL_FN(PFNGLBINDTEXTUREPROC, glBindTexture, "glBindTexture");
  GET_GL_FN(PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D, "glCompressedTexImage2D");
  GET_GL_FN(PFNGLDELETETEXTURESPROC, glDeleteTextures, "glDeleteTextures");
  GET_GL_FN(PFNGLGENTEXTURESPROC, glGenTextures, "glGenTextures");
  GET_GL_FN(PFNGLTEXIMAGE2DPROC, glTexImage2D, "glTexImage2D");
  GET_GL_FN(PFNGLTEXPARAMETERFPROC, glTexParameterf, "glTexParameterf");
  GET_GL_FN(PFNGLTEXPARAMETERIPROC, glTexParameteri, "glTexParameteri");
  GET_GL_FN(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray, "glDisableVertexAttribArray");
  GET_GL_FN(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray, "glEnableVertexAttribArray");
  GET_GL_FN(PFNGLBINDBUFFERPROC, glBindBuffer, "glBindBuffer");
  GET_GL_FN(PFNGLBUFFERDATAPROC, glBufferData, "glBufferData");
  GET_GL_FN(PFNGLDELETEBUFFERSPROC, glDeleteBuffers, "glDeleteBuffers");
  GET_GL_FN(PFNGLGENBUFFERSPROC, glGenBuffers, "glGenBuffers");
  GET_GL_FN(PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv, "glGetBufferParameteriv");
  GET_GL_FN(PFNGLMAPBUFFERPROC, glMapBuffer, "glMapBuffer");
  GET_GL_FN(PFNGLUNMAPBUFFERPROC, glUnmapBuffer, "glUnmapBuffer");
  GET_GL_FN(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer, "glVertexAttribPointer");
  GET_GL_FN(PFNGLCREATEPROGRAMPROC, glCreateProgram, "glCreateProgram");
  GET_GL_FN(PFNGLDELETEPROGRAMPROC, glDeleteProgram, "glDeleteProgram");
  GET_GL_FN(PFNGLGETPROGRAMIVPROC, glGetProgramiv, "glGetProgramiv");
  GET_GL_FN(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog, "glGetProgramInfoLog");
  GET_GL_FN(PFNGLLINKPROGRAMPROC, glLinkProgram, "glLinkProgram");
  GET_GL_FN(PFNGLUSEPROGRAMPROC, glUseProgram, "glUseProgram");
  GET_GL_FN(PFNGLATTACHSHADERPROC, glAttachShader, "glAttachShader");
  GET_GL_FN(PFNGLCOMPILESHADERPROC, glCompileShader, "glCompileShader");
  GET_GL_FN(PFNGLCREATESHADERPROC, glCreateShader, "glCreateShader");
  GET_GL_FN(PFNGLGETSHADERIVPROC, glGetShaderiv, "glGetShaderiv");
  GET_GL_FN(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog, "glGetShaderInfoLog");
  GET_GL_FN(PFNGLSHADERSOURCEPROC, glShaderSource, "glShaderSource");
  GET_GL_FN(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation, "glBindAttribLocation");
  GET_GL_FN(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation, "glGetUniformLocation");
  GET_GL_FN(PFNGLUNIFORM1IPROC, glUniform1i, "glUniform1i");
  GET_GL_FN(PFNGLUNIFORM2FPROC, glUniform2f, "glUniform2f");
  GET_GL_FN(PFNGLUNIFORM3FPROC, glUniform3f, "glUniform3f");
  GET_GL_FN(PFNGLUNIFORM4FPROC, glUniform4f, "glUniform4f");
  GET_GL_FN(PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv, "glUniformMatrix3fv");
  GET_GL_FN(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv, "glUniformMatrix4fv");
  GET_GL_FN(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex, "glGetUniformBlockIndex");
  GET_GL_FN(PFNGLGETACTIVEUNIFORMBLOCKIVPROC, glGetActiveUniformBlockiv, "glGetActiveUniformBlockiv");
  GET_GL_FN(PFNGLBINDBUFFERBASEPROC, glBindBufferBase, "glBindBufferBase");
  GET_GL_FN(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding, "glUniformBlockBinding");
  GET_GL_FN(PFNGLGETUNIFORMINDICESPROC, glGetUniformIndices, "glGetUniformIndices");
  GET_GL_FN(PFNGLGETACTIVEUNIFORMSIVPROC, glGetActiveUniformsiv, "glGetActiveUniformsiv");
  GET_GL_FN(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer, "glBindFramebuffer");
  GET_GL_FN(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers, "glDeleteFramebuffers");
  GET_GL_FN(PFNGLDRAWBUFFERPROC, glDrawBuffer, "glDrawBuffer");
  GET_GL_FN(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D, "glFramebufferTexture2D");
  GET_GL_FN(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers, "glGenFramebuffers");
  GET_GL_FN(PFNGLREADBUFFERPROC, glReadBuffer, "glReadBuffer");

  return status;
}