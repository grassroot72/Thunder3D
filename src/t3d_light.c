/*----- t3d_light.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_texture.h>
#include <t3d_math.h>
#include <t3d_object.h>
#include <t3d_frustum.h>
#include <t3d_light.h>


/* generate an empty shadowmap texture */
static GLuint
lit_gen_depth_tex( const int shadow_size )
{
  GLuint tex_id;

  glGenTextures(1, &tex_id);
  glBindTexture(GL_TEXTURE_2D, tex_id);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
               shadow_size, shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  return tex_id;
}

/* create light struct in memory */
light *
lit_new( const int shadow_size, const vec3 *pos, const vec3 *lookat )
{
  light *lit = (light *)malloc(sizeof(light));

  lit->shadow_size = shadow_size;
  lit->tex_shadow = lit_gen_depth_tex(shadow_size);
  tex_set_flags(lit->tex_shadow, TEX_USE_LINEAR|TEX_TO_BORDER);

  /* Framebuffer for the shadow map */
  glGenFramebuffers(1, &lit->fbo_shadow);
  glBindFramebuffer(GL_FRAMEBUFFER, lit->fbo_shadow);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lit->tex_shadow, 0);

  /* set the light move speed */
  lit->move_speed = 0.0;
  lit->time = 0.0;

  vec3_cpy(&lit->pos, pos);
  vec3_cpy(&lit->lookat, lookat);
  vec3_set(&lit->u_ref, 0.0, 1.0, 0.0);

  mat4_set(&lit->mat_bias, 0.5, 0.0, 0.0, 0.0,
                           0.0, 0.5, 0.0, 0.0,
                           0.0, 0.0, 0.5, 0.0,
                           0.5, 0.5, 0.5, 1.0);

	mat4_set_ortho(&lit->mat_proj, -384.0, 384.0, -384.0, 384.0, -2580.0, 2580.0);
  mat4_cpy(&lit->mat_ortho, &lit->mat_proj);

  return lit;
}

/* destroy light struct */
void
lit_del( light *lit )
{
  if(!lit) return;

  glDeleteFramebuffers(1, &lit->fbo_shadow);
  glDeleteTextures(1, &lit->tex_shadow);

  free(lit);
}

/* set light's position and lookat */
void
lit_set_view( light *lit )
{
  /* for view matrix, need to negate the forward vector, so we use
     light's (forward = pos - lookat) */
  obj_find_f_axis(&lit->f, &lit->lookat, &lit->pos);
  obj_find_axis(&lit->r, &lit->u_ref, &lit->f);
  obj_find_axis(&lit->u, &lit->f, &lit->r);
  mat4_set_view(&lit->mat_view, &lit->pos, &lit->f, &lit->u, &lit->r);
}