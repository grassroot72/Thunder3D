/*----- t3d_geopatch.h -------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_geopatch_h_
#define _t3d_geopatch_h_

#include <GL/glcorearb.h>
#include <t3d_type.h>


struct __geopatch {
  int visible;                  /* if it is visible to light and camera */

  vec3 center;                  /* center of the AABBox */
  float half_x_len;             /* AABBox size in x direction */
  float half_y_len;             /* AABBox size in y direction */
  float half_z_len;             /* AABBox size in z direction */

  GLuint vbo_coords;            /* patch specific - VBO vertex coordinates */
  GLuint vbo_normals;           /* patch specific - VBO vertex normals */
  GLuint vbo_tangents;          /* patch specific - VBO vertex tangents */

  const GLuint *tex_diffuse;    /* patch specific - diffuse texture */
  const GLuint *tex_normalmap;  /* patch specific - normalmap texture */
  const GLuint *tex_specular;   /* patch specific - specular texture */
};


/* create a geopatch struct in memory */
geopatch *geopch_new( void );
/* delete a geopatch struct form memory */
void geopch_del( geopatch *pch );
/* store vertices, normals tangents in graphic card buffers */
void geopch_upload( const geopatch *pch,
                    const vec3 *vcoords,
                    const vec3 *normals,
                    const vec3 *tangents,
                    const unsigned int v_data_size );


#endif   /* _t3d_geopatch_h_ */
