/*----- t3d_aamesh.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#ifndef _t3d_aamesh_h_
#define _t3d_aamesh_h_

#include <t3d_type.h>


struct __aamesh {
  vec3 *vcoords;           /* vertex indices of the decal mesh */
  vec2 tex_offset;         /* offset to translate the texture coordinate */

  int v_data_size;         /* vertices data size */

  const GLuint *tex_id;

  GLuint vbo_coord;        /* VBO - aamesh vertices */

  mat4 mat_model;          /* model matrix */
};


/* create a decal target mesh in memory */
aamesh *aamesh_new( const decal *d );
/* delete a decal from memory */
void aamesh_del( aamesh *aamsh );
/* set the axis aligned decal texture */
void aamesh_set_tex( aamesh *aamsh, const hashtable *texdb );
/* find patchmap vertices of the decal patch */
void aamesh_get_mesh( aamesh *aamsh,
                      const decal *d,
                      const int x,
                      const int y,
                      const patchmap *pm );
/* find aamesh texture offset */
void aamesh_tex_offset( aamesh *aamsh,
                        const decal *d,
                        const float x,    /* unit's 2d postion x on patchmap */
                        const float y,    /* unit's 2d postion y on patchmap */
                        const float htmap_step );
/* update vbo */
void aamesh_update_vbo( const aamesh *aamsh );
/* draw the aamesh */
void aamesh_draw( aamesh *aamsh,
                  const decal *d,
                  const camera *cam,
                  const vec4 *color,
                  shader *glsl );


#endif   /* _t3d_aamesh_h_ */