/*----- t3d_frustum.c --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <t3d_math.h>
#include <t3d_frustum.h>


/* create a frustum */
void
frustum_set( frustum *f,
             const float fovy,   /* in degree */
             const float aspect, /* aspect 4:3, 16:9 etc. */
             const float near,   /* z-near */
             const float far )   /* z-far */
{
  float rad = DEG2RAD(fovy*0.5);
  float tangent = tanf(rad);
  float cosine = cosf(rad);
  f->half_near_h = near*tangent;            /* half height of near plane */
  f->half_near_w = f->half_near_h*aspect;   /* half width of near plane */
  f->half_far_h = far*tangent;              /* half height of far plane */
  f->half_far_w = f->half_far_h*aspect;     /* half width of far plane */

  float side_len = far/cosine;
  f->max_distsq = side_len*side_len + f->half_far_w*f->half_far_w;
}

/* calculate camera frustum */
void
frustum_set_clip_planes( vec4 *clip_planes, const mat4 *clip )
{
  /* the matrix clip = proj * view */

  /* RIGHT */
  clip_planes[0].A = clip->m03 - clip->m00;
  clip_planes[0].B = clip->m13 - clip->m10;
  clip_planes[0].C = clip->m23 - clip->m20;
  clip_planes[0].D = clip->m33 - clip->m30;
  vec4_norm(&clip_planes[0]);

  /* LEFT */
  clip_planes[1].A = clip->m03 + clip->m00;
  clip_planes[1].B = clip->m13 + clip->m10;
  clip_planes[1].C = clip->m23 + clip->m20;
  clip_planes[1].D = clip->m33 + clip->m30;
  vec4_norm(&clip_planes[1]);

  /* BOTTOM */
  clip_planes[2].A = clip->m03 + clip->m01;
  clip_planes[2].B = clip->m13 + clip->m11;
  clip_planes[2].C = clip->m23 + clip->m21;
  clip_planes[2].D = clip->m33 + clip->m31;
  vec4_norm(&clip_planes[2]);

  /* TOP */
  clip_planes[3].A = clip->m03 - clip->m01;
  clip_planes[3].B = clip->m13 - clip->m11;
  clip_planes[3].C = clip->m23 - clip->m21;
  clip_planes[3].D = clip->m33 - clip->m31;
  vec4_norm(&clip_planes[3]);

  /* FAR */
  clip_planes[4].A = clip->m03 - clip->m02;
  clip_planes[4].B = clip->m13 - clip->m12;
  clip_planes[4].C = clip->m23 - clip->m22;
  clip_planes[4].D = clip->m33 - clip->m32;
  vec4_norm(&clip_planes[4]);

  /* NEAR */
  clip_planes[5].A = clip->m03 + clip->m02;
  clip_planes[5].B = clip->m13 + clip->m12;
  clip_planes[5].C = clip->m23 + clip->m22;
  clip_planes[5].D = clip->m33 + clip->m32;
  vec4_norm(&clip_planes[5]);
}

/* check if a point is in the frustum */
int
frustum_is_point_in( const vec4 *clip_planes, const float x, const float y, const float z )
{
  int i;

  for(i = 0; i < 6; i++) {
    if(clip_planes[i].A*x + clip_planes[i].B*y + clip_planes[i].C*z + clip_planes[i].D <= 0.0)
      return 0;
  }
  return 1;
}

/* check if a box is in the frustum */
int
frustum_is_box_in( const vec4 *clip_planes, 
                   const float x,            /* center x */
                   const float y,            /* center y */
                   const float z,            /* center z */
                   const float half_size_x,
                   const float half_size_y,
                   const float half_size_z )
{
  int i;
  /* loop through six vertices of the box against the view frustum */
  for(i = 0; i < 6; i++) {
    if(clip_planes[i].A*(x - half_size_x) +
       clip_planes[i].B*(y - half_size_y) +
       clip_planes[i].C*(z - half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x + half_size_x) +
       clip_planes[i].B*(y - half_size_y) +
       clip_planes[i].C*(z - half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x - half_size_x) +
       clip_planes[i].B*(y + half_size_y) +
       clip_planes[i].C*(z - half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x + half_size_x) +
       clip_planes[i].B*(y + half_size_y) +
       clip_planes[i].C*(z - half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x - half_size_x) +
       clip_planes[i].B*(y - half_size_y) +
       clip_planes[i].C*(z + half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x + half_size_x) +
       clip_planes[i].B*(y - half_size_y) +
       clip_planes[i].C*(z + half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x - half_size_x) +
       clip_planes[i].B*(y + half_size_y) +
       clip_planes[i].C*(z + half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    if(clip_planes[i].A*(x + half_size_x) +
       clip_planes[i].B*(y + half_size_y) +
       clip_planes[i].C*(z + half_size_z) +
       clip_planes[i].D > 0.0)  continue;

    return 0;   /* If we get here, it isn't in the frustum */
  }
  return 1;
}

/* check if a sphere is in the frustum */
int
frustum_is_sphere_in( const vec4 *clip_planes,
                      const float x,         /* center x */
                      const float y,         /* center y */
                      const float z,         /* center z */
                      const float radius )
{
  int i;
  /* calculate the distances to each of the planes */
  for(i = 0; i < 6; ++i) {
    if(clip_planes[i].A*x +
       clip_planes[i].B*y +
       clip_planes[i].C*z +
       clip_planes[i].D <= -radius)  return 0;
  }
  return 1;   /* we are fully in view */
}