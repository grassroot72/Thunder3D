/*----- t3d_patchmap.c ------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_hashtable.h>
#include <t3d_object.h>
#include <t3d_light.h>
#include <t3d_texture.h>
#include <t3d_heightmap.h>
#include <t3d_geopatch.h>
#include <t3d_patch.h>
#include <t3d_patchmap.h>


/* find the geopatch vertex index in the patchmap  */
static unsigned int
pchmap_find_v_index( const patchmap *pm,
                     const int patch_x, const int patch_z, /* patch position */
                     const int x, const int z )
{
  return (z + pm->w_pch_cells*patch_z)*pm->w + (x + pm->h_pch_cells*patch_x);    
}

/* copy heightmap vertices to patches */
static void
pchmap_load_patches( patchmap *pm, const hashtable *texdb )
{
  float pch_x_len = pm->w_pch_cells*pm->step;
  float pch_z_len = pm->h_pch_cells*pm->step;
  float pch_half_x_len = pch_x_len*0.5;
  float pch_half_z_len = pch_z_len*0.5;

  float pch_y;
  int pch_count = 0;

  unsigned int v_index;
  unsigned int v_data_size;
  int v_count;

  vec3 *vcoords = (vec3 *)malloc(sizeof(vec3)*pm->n_total_pch_vertices);
  vec3 *normals = (vec3 *)malloc(sizeof(vec3)*pm->n_total_pch_vertices);
  vec3 *tangents = (vec3 *)malloc(sizeof(vec3)*pm->n_total_pch_vertices);

  int x, z, i, j;
  for(j = 0; j < pm->h_pchs; j++) {
    for(i = 0; i < pm->w_pchs; i++) {
      pm->pchs[pch_count] = geopch_new();
      geopatch *pch = pm->pchs[pch_count];

      /* loop thru the vertices of a patch */
      pch_y = 0.0;
      v_count = 0;
      for(z = 0; z < pm->pch_h; z++) {
        for(x = 0; x < pm->pch_w; x++) {
          /* find the vertex index on the patch map */
          v_index = pchmap_find_v_index(pm, i, j, x, z);

          vec3_cpy(&vcoords[v_count], &pm->vcoords[v_index]);
          vec3_cpy(&normals[v_count], &pm->normals[v_index]);
          vec3_cpy(&tangents[v_count], &pm->tangents[v_index]);

          pch_y += pm->vcoords[v_index].y;
          v_count++;
        }
      }

      v_data_size = pm->n_total_pch_vertices*sizeof(vec3);
      /* upload geopatch to video memory */
      geopch_upload(pch, vcoords, normals, tangents, v_data_size);

      /* calculate the center & half_size of the patch */
      pch->half_x_len = pch_half_x_len;
      pch->half_z_len = pch_half_z_len;
      pch->center.x = pch_x_len*i + pch_half_x_len;
      pch->center.y = (pch_y/pm->n_total_pch_vertices)*0.5;
      pch->center.z = pch_z_len*j + pch_half_z_len;
      pch->half_y_len = pch->center.y;

      /* load the textures */
      pch->tex_diffuse = hash_get(texdb, "floor_diffuse.jpg");
      pch->tex_normalmap = hash_get(texdb, "floor_normalmap.png");
      pch->tex_specular = hash_get(texdb, "floor_spec.jpg");
      tex_set_flags(*pch->tex_diffuse, TEX_USE_MIPMAPS|TEX_TO_EDGE);
      tex_set_flags(*pch->tex_normalmap, TEX_USE_LINEAR|TEX_TO_EDGE);
      tex_set_flags(*pch->tex_specular, TEX_USE_MIPMAPS|TEX_TO_EDGE);

      pch_count++;
    }
  }

  free(vcoords);
  free(normals);
  free(tangents);
}

/* store common vertex coordinates, elements in graphic card buffers */
static void
pchmap_upload( const patchmap *pm )
{
  glBindBuffer(GL_ARRAY_BUFFER, pm->vbo_texcoords);
  glBufferData(GL_ARRAY_BUFFER,
               pm->n_total_pch_vertices*sizeof(vec2),
               pm->pch_texcoords,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm->ibo_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               pm->n_total_pch_indices*sizeof(unsigned int),
               pm->pch_indices,
               GL_STATIC_DRAW);
}

/* creata a patchmap struct in memory */
patchmap *
pchmap_new( const char *file,
            const int patch_w,
            const int patch_h,
            const float htmap_step,
            const float htmap_scale,
            const hashtable *texdb )
{
  patchmap *pm = (patchmap *)malloc(sizeof(patchmap));

  /* heightmap related data */
  unsigned char *raw_heights;
  raw_heights = htmap_from_file(file, &pm->w,
                                      &pm->h,
                                      &pm->n_total_vertices);

  pm->step = htmap_step;
  pm->scale = htmap_scale;

  /* cell related data */
  pm->w_cells = pm->w - 1;
  pm->h_cells = pm->h - 1;
  pm->n_total_cells = pm->w_cells*pm->h_cells;

  /* patch related data */
  pm->pch_w = patch_w;
  pm->pch_h = patch_h;
  pm->n_total_pch_vertices = patch_w*patch_h;

  pm->w_pch_cells = patch_w - 1;
  pm->h_pch_cells = patch_h - 1;
  pm->n_total_pch_cells = pm->w_pch_cells*pm->h_pch_cells;

  pm->w_pchs = pm->w_cells/pm->w_pch_cells;
  pm->h_pchs = pm->h_cells/pm->h_pch_cells;
  pm->n_total_pchs = pm->w_pchs*pm->h_pchs;

  /* number of patch indices */
  int n_strips_required = pm->h_pch_cells;
  int n_degens_required = 2*(n_strips_required - 1);
  int n_vertices_per_strip = 2*patch_w;
  pm->n_total_pch_indices = n_vertices_per_strip*n_strips_required +
                            n_degens_required;


  /* allocate memory for every elements */
  /* vertex attributes */
  pm->vcoords = (vec3 *)malloc(sizeof(vec3)*pm->n_total_vertices);
  pm->normals = (vec3 *)malloc(sizeof(vec3)*pm->n_total_vertices);
  pm->tangents = (vec3 *)malloc(sizeof(vec3)*pm->n_total_vertices);

  /* cell related */
  pm->cells = (cell *)malloc(sizeof(cell)*pm->n_total_cells);

  vec2 *texcoords = (vec2 *)malloc(sizeof(vec2)*pm->n_total_vertices);
  htmap_load_vertices(pm->vcoords, pm->normals, pm->tangents, texcoords,
                      raw_heights, pm->w, pm->h, pm->step, pm->scale);
  /* find the tangents, normals in the heightmap */
  htmap_find_tangents(pm->tangents, pm->normals, pm->cells,
                      pm->vcoords, texcoords, pm->n_total_vertices,
                      pm->w, pm->h, pm->w_cells, pm->h_cells);
  free(texcoords);

  /* patch related */
  pm->pch_texcoords = (vec2 *)malloc(sizeof(vec2)*pm->n_total_pch_vertices);
  /* find the texture coordinates of a geopatch */
  patch_find_texcoords(pm->pch_texcoords, patch_w, patch_h);

  pm->pch_indices = (unsigned int *)malloc(sizeof(unsigned int)*pm->n_total_pch_indices);
  /* indexing the geopatch */
  patch_v_indexing(pm->pch_indices, patch_w, patch_h);

  /* patch related */
  pm->pchs = (geopatch **)malloc(sizeof(geopatch *)*pm->n_total_pchs);
  /* generate vbo, ibo buffers */
  glGenBuffers(1, &pm->vbo_texcoords);
  glGenBuffers(1, &pm->ibo_elements);
  pchmap_load_patches(pm, texdb);
  pchmap_upload(pm);

  free(raw_heights);

  /* initialize the mutex lock for patchmap */
  pthread_mutex_init(&pm->lock, NULL);

  mat4_identity(&pm->mat_model);
  mat3_frm_mat4(&pm->mat_m3x3, &pm->mat_model);

  return pm;
}

/* delete a patchmap struct from memory */
void
pchmap_del( patchmap *pm )
{
  if(!pm) return;

  glDeleteBuffers(1, &pm->vbo_texcoords);
  glDeleteBuffers(1, &pm->ibo_elements);

  int i;
  for(i = 0; i < pm->n_total_pchs; i++) {
    geopatch *pch = pm->pchs[i];
    geopch_del(pch);
  }
  free(pm->pchs);

  free(pm->pch_texcoords);
  free(pm->pch_indices);

  pthread_mutex_destroy(&pm->lock);

  free(pm);
}

/* update patchmap visibility against frustum 
void
pchmap_update_visi( patchmap *pm, const camera *cam, const light *lit )
{
  int i;
  for(i = 0; i < pm->n_total_pchs; i++) {
    geopatch *pch = pm->pchs[i];

    vec3 dist;
    vec3_sub(&dist, &pch->center, &cam->lens->pos);
    if(vec3_lensq(&dist) < cam->v_frustum->max_distsq) {

      if(frustum_is_box_in(cam->clips,
                           pch->center.x,
                           pch->center.y,
                           pch->center.z,
                           pch->half_x_len,
                           pch->half_y_len,
                           pch->half_z_len)) {

        if(frustum_is_box_in(lit->clips,
                             pch->center.x,
                             pch->center.y,
                             pch->center.z,
                             pch->half_x_len,
                             pch->half_y_len,
                             pch->half_z_len))
          pch->visible = 1;
        else
          pch->visible = 0;
      }
      else
        pch->visible = 0;
    }
    else
      pch->visible = 0;
  }
}
*/

/* draw the patchmap with normalmap */
void
pchmap_normalmap_draw( const patchmap *pm, const light *lit, const shader *glsl )
{
  int i;

  glEnableVertexAttribArray(glsl->attri_v_coord);
  glEnableVertexAttribArray(glsl->attri_v_normal);
  glEnableVertexAttribArray(glsl->attri_v_tangent);
  glEnableVertexAttribArray(glsl->attri_v_texcoord);

  for(i = 0; i < pm->n_total_pchs; i++) {
    geopatch *pch = pm->pchs[i];

    if(pch->visible) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, *pch->tex_diffuse);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, *pch->tex_normalmap);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, *pch->tex_specular);

      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, lit->tex_shadow);


      glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_coords);
      glVertexAttribPointer(glsl->attri_v_coord,   /* attribute */
                            3,                     /* (x, y, z) */
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            0);

      glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_normals);
      glVertexAttribPointer(glsl->attri_v_normal,   /* attribute */
                            3,                      /* (x, y, z) */
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            0);

      glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_tangents);
      glVertexAttribPointer(glsl->attri_v_tangent,  /* attribute */
                            3,                      /* (x, y, z) */
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            0);

      glBindBuffer(GL_ARRAY_BUFFER, pm->vbo_texcoords);
      glVertexAttribPointer(glsl->attri_v_texcoord,  /* attribute */
                            2,                       /* (x, y) */
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            0);

      /* push each element to the vertex shader */
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm->ibo_elements);
      glDrawElements(GL_TRIANGLE_STRIP, pm->n_total_pch_indices, GL_UNSIGNED_INT, 0);
    }
  }

  glDisableVertexAttribArray(glsl->attri_v_texcoord);
  glDisableVertexAttribArray(glsl->attri_v_tangent);
  glDisableVertexAttribArray(glsl->attri_v_normal);
  glDisableVertexAttribArray(glsl->attri_v_coord);
}

/* draw the patchmap with one vertex coordinates */
void
pchmap_vertex_draw( const patchmap *pm, const shader *glsl )
{
  int i;

  glEnableVertexAttribArray(glsl->attri_v_coord);

  for(i = 0; i < pm->n_total_pchs; i++) {
    geopatch *pch = pm->pchs[i];

    if(pch->visible) {
      glBindBuffer(GL_ARRAY_BUFFER, pch->vbo_coords);
      glVertexAttribPointer(glsl->attri_v_coord,   /* attribute */
                            3,                     /* (x, y, z) */
                            GL_FLOAT,
                            GL_FALSE,
                            0,
                            0);

      /* push each element to the vertex shader */
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pm->ibo_elements);
      glDrawElements(GL_TRIANGLE_STRIP, pm->n_total_pch_indices, GL_UNSIGNED_INT, 0);
    }
  }

  glDisableVertexAttribArray(glsl->attri_v_coord);
}