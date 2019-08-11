/*----- t3d_ogl.h ------------------------------------------------------------+
    Developped by: Edward LEI
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_ogl_h_
#define _t3d_ogl_h_

#include <GL/glcorearb.h>


/* common functions */
PFNGLBLENDFUNCPROC     glBlendFunc;
PFNGLCLEARPROC         glClear;
PFNGLCLEARCOLORPROC    glClearColor;
PFNGLCOLORMASKPROC     glColorMask;
PFNGLCULLFACEPROC      glCullFace;
PFNGLDEPTHFUNCPROC     glDepthFunc;
PFNGLDEPTHMASKPROC     glDepthMask;
PFNGLDISABLEPROC       glDisable;
PFNGLDRAWARRAYSPROC    glDrawArrays;
PFNGLDRAWELEMENTSPROC  glDrawElements;
PFNGLENABLEPROC        glEnable;
PFNGLGETFLOATVPROC     glGetFloatv;
PFNGLPOLYGONMODEPROC   glPolygonMode;
PFNGLPOLYGONOFFSETPROC glPolygonOffset;
PFNGLVIEWPORTPROC      glViewport;
/* Texture functions */
PFNGLACTIVETEXTUREPROC        glActiveTexture;
PFNGLBINDTEXTUREPROC          glBindTexture;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLDELETETEXTURESPROC       glDeleteTextures;
PFNGLGENTEXTURESPROC          glGenTextures;
PFNGLTEXIMAGE2DPROC           glTexImage2D;
PFNGLTEXPARAMETERFPROC        glTexParameterf;
PFNGLTEXPARAMETERIPROC        glTexParameteri;
/* VAO, VBO functions */
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
PFNGLBINDBUFFERPROC               glBindBuffer;
PFNGLBUFFERDATAPROC               glBufferData;
PFNGLDELETEBUFFERSPROC            glDeleteBuffers;
PFNGLGENBUFFERSPROC               glGenBuffers;
PFNGLGETBUFFERPARAMETERIVPROC     glGetBufferParameteriv;
PFNGLMAPBUFFERPROC                glMapBuffer;
PFNGLUNMAPBUFFERPROC              glUnmapBuffer;
PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
/* GLSL */
/* program functions */
PFNGLCREATEPROGRAMPROC     glCreateProgram;
PFNGLDELETEPROGRAMPROC     glDeleteProgram;
PFNGLGETPROGRAMIVPROC      glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLLINKPROGRAMPROC       glLinkProgram;
PFNGLUSEPROGRAMPROC        glUseProgram;
/* shader functions */
PFNGLATTACHSHADERPROC     glAttachShader;
PFNGLCOMPILESHADERPROC    glCompileShader;
PFNGLCREATESHADERPROC     glCreateShader;
PFNGLGETSHADERIVPROC      glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLSHADERSOURCEPROC     glShaderSource;
/* attribute, uniform functions */
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC          glUniform1i;
PFNGLUNIFORM2FPROC          glUniform2f;
PFNGLUNIFORM3FPROC          glUniform3f;
PFNGLUNIFORM4FPROC          glUniform4f;
PFNGLUNIFORMMATRIX3FVPROC   glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv;
/* UBO */
PFNGLGETUNIFORMBLOCKINDEXPROC    glGetUniformBlockIndex;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
PFNGLBINDBUFFERBASEPROC          glBindBufferBase;
PFNGLUNIFORMBLOCKBINDINGPROC     glUniformBlockBinding;
PFNGLGETUNIFORMINDICESPROC       glGetUniformIndices;
PFNGLGETACTIVEUNIFORMSIVPROC     glGetActiveUniformsiv;
/* FBO */
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLDRAWBUFFERPROC glDrawBuffer;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLREADBUFFERPROC glReadBuffer;


/* resolve the opengl functions */
GLboolean ogl_init( void );


#endif	/* _t3d_ogl_h_ */