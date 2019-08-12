/*----- t3d_minimap.h -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_minimap_h_
#define _t3d_minimap_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


struct __minimap {
  const GLuint *tex_id;

  GLuint vbo_coord;        /* VBO - vertex coordinate + texture coordinate */
  mat4 mat_proj;           /* projection (otho) matrix */
};


/* create minimap struct in memory */
minimap *minimap_new( const char *mapname,
                      const int screen_w,
                      const int screen_h,
                      const hashtable *texdb );
/* delete minimap struct from memory */
void minimap_del( minimap *mm );
/* draw the minimap */
void minimap_draw( minimap *mm, shader *glsl );



#endif   /* _t3d_minimap_h_ */