/*----- t3d_heightmap.c ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stb_image.h>
#include <t3d_math.h>
#include <t3d_geomath.h>
#include <t3d_astar.h>
#include <t3d_patchmap.h>
#include <t3d_heightmap.h>


/* load heightmap from file */
unsigned char *
htmap_from_file( const char *file, int *w, int *h, int *n_vertices )
{
  unsigned char *img;
  int channels;
  int i;

  img = stbi_load(file, w, h, &channels, 0);
  *n_vertices = (*w)*(*h);
  unsigned char *data = (unsigned char *)malloc(sizeof(unsigned char)*(*n_vertices));

  for(i = 0 ; i < (*n_vertices); i++) {
    /* greyscale format */
    if(channels == 1)
      data[i] = img[i];
    else
      /* convert pixels: greyscale = 0.30*R + 0.59*G + 0.11*B */
      data[i] = (unsigned char)(0.30*img[i*3 + 0] +
                                0.59*img[i*3 + 1] +
                                0.11*img[i*3 + 2]);
  }
  stbi_image_free(img);

  return data;
}

/* fill vertex position and texture coordinates */
void
htmap_load_vertices( vec3 *vcoords,
                     vec3 *normals,
                     vec3 *tangents,
                     vec2 *texcoords,
                     const unsigned char *raw_heights,
                     const int htmap_w,
                     const int htmap_h,
                     const float htmap_step,
                     const float htmap_scale )
{
  int x, z;
  unsigned int count = 0;

  for(z = 0; z < htmap_h; z++) {
    for(x = 0; x < htmap_w; x++) {
      vcoords[count].x = x*htmap_step;
      vcoords[count].y = raw_heights[(z*htmap_w) + x]*htmap_scale;
      vcoords[count].z = z*htmap_step;

      texcoords[count].s = x/(float)(htmap_w - 1);
      texcoords[count].t = z/(float)(htmap_h - 1);

      vec3_set(&normals[count], 0.0, 0.0, 0.0);
      vec3_set(&tangents[count], 0.0, 0.0, 0.0);
      count++;
    }
  }
}

/* calculate tangents, normals and cell planes of the heightmap */
void
htmap_find_tangents( vec3 *tangents,
                     vec3 *normals,
                     cell *cells,
                     const vec3 *vcoords,
                     const vec2 *texcoords,
                     const int n_total_vertices,
                     const int htmap_w,
                     const int htmap_h,
                     const int w_cells,
                     const int h_cells )
{
  int x, z;
  unsigned int i0, i1, i2, i, count = 0;
  const vec3 *v0, *v1, *v2;
  const vec2 *uv0, *uv1, *uv2;
  vec3 triangle_normal;
  vec3 tangent;
  float dist;

  /* loop thru the triangles of heightmap */
  for(z = 0; z < h_cells; z++) {
    for(x = 0; x < w_cells; x++) {
      /*----- 1ST TRIANGLE in a cell -----------------------------------------+
         i0--i2
          | / |
	        |/  |
	       i1---+
       +----------------------------------------------------------------------*/
      /* indices - 1st triangle */
      i0 = z*htmap_w + x;
      i1 = (z + 1)*htmap_w + x;
      i2 = i0 + 1;
      /* vertex coordinates - 1st triangle */
      v0 = &vcoords[i0];
      v1 = &vcoords[i1];
      v2 = &vcoords[i2];
      /* texture coordinates - 1st triangle */
      uv0 = &texcoords[i0];
      uv1 = &texcoords[i1];
      uv2 = &texcoords[i2];
       /* v0, v1, v2 tri_normal */
      gmth_find_normal(&triangle_normal, v0, v1, v2);
      dist = -vec3_dot(&triangle_normal, v0);
      /* calculate the first plane of the cell */
      vec4_frm_v3w(&cells[count].planes[0], &triangle_normal, dist);
      /* find the center of the cell */
      vec3_lerp(&cells[count].center, v1, v2, 0.5);
      /* vertex tangent */
      gmth_find_tangent(&tangent, v0, v1, v2, uv0, uv1, uv2);

      vec3_self_add(&normals[i0], &triangle_normal);        /* v0 normal */
      vec3_self_add(&normals[i1], &triangle_normal);        /* v1 normal */
      vec3_self_add(&normals[i2], &triangle_normal);        /* v2 normal */

      vec3_self_add(&tangents[i0], &tangent);               /* v0 tangent */
      vec3_self_add(&tangents[i1], &tangent);               /* v1 tangent */
      vec3_self_add(&tangents[i2], &tangent);               /* v2 tangent */

      /*----- 2ND TRIANGLE in a cell -----------------------------------------+
          +--i0
          | / |
	        |/  |
	       i1--i2
       +----------------------------------------------------------------------*/
      /* indices - 2nd triangle */
      i0 = i2;
      i2 = i1 + 1;
      /* vertex coordinates - 2nd triangle */
      v0 = &vcoords[i0];
      v2 = &vcoords[i2];
      /* texture coordinates - 2nd triangle */
      uv0 = &texcoords[i0];
      uv2 = &texcoords[i2];
      /* v0, v1, v2 tri_normal */
      gmth_find_normal(&triangle_normal, v0, v1, v2);
      dist = -vec3_dot(&triangle_normal, v0);
      /* calculate the first plane of the cell */
      vec4_frm_v3w(&cells[count].planes[1], &triangle_normal, dist);
      /* vertex tangent */
      gmth_find_tangent(&tangent, v0, v1, v2, uv0, uv1, uv2);

      vec3_self_add(&normals[i0], &triangle_normal);        /* v0 normal */
      vec3_self_add(&normals[i1], &triangle_normal);        /* v1 normal */
      vec3_self_add(&normals[i2], &triangle_normal);        /* v2 normal */

      vec3_self_add(&tangents[i0], &tangent);               /* v0 tangent */
      vec3_self_add(&tangents[i1], &tangent);               /* v1 tangent */
      vec3_self_add(&tangents[i2], &tangent);               /* v2 tangent */

      /*---- cell AStar status -----------------------------------------------*/
      cells[count].status = ASTAR_AVAIL;
      count++;
    }
  }

  /* normalize the vertex normals and tangents */
  for(i = 0; i < n_total_vertices; i++) {
    vec3_norm(&normals[i]);
    vec3_norm(&tangents[i]);
  }
}