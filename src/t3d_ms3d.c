/*----- t3d_ms3d.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <t3d_math.h>
#include <t3d_ogl.h>
#include <t3d_vector.h>
#include <t3d_util.h>
#include <t3d_texture.h>
#include <t3d_geomath.h>
#include <t3d_ms3dmesh.h>
#include <t3d_decal.h>
#include <t3d_ms3d.h>


typedef struct __ms3dkey ms3dkey;

struct __ms3dkey {
  float time;   /* in milliseconds */
  vec3 param;   /* angles in radiance */
};

struct __ms3djoint {
  mat4 mat_abs;
  mat4 mat_local;

  int parent;

  unsigned short n_rotkeys;    /* number of Rotation Key frames */
  unsigned short n_transkeys;  /* number of Translation Key frames */
  ms3dkey *rotkeys;
  ms3dkey *transkeys;
};


/* the following packed structs are only used for load ms3d file */

/* byte-aligned structures */
#ifdef _MSC_VER
#  pragma pack(push, packing)
#  pragma pack(1)
#  define __MS3DPACKED
#elif defined(__GNUC__)
#  define __MS3DPACKED __attribute__ ((gcc_struct, __packed__))
#else
#  error you must byte-align these structures
#endif

/* Milkshape3d File header information */
typedef struct __MS3DPACKED {
  char id[10];
  int version;
} __ms3dheader;

/* Vertex information */
typedef struct __MS3DPACKED {
  unsigned char flags;
  vec3 pos;       /* vertex position */
  char bone_id;   /* bone id */
  unsigned char ref_count;
} __ms3dvertex;

/* Triangle information */
typedef struct __MS3DPACKED {
  unsigned short flags;
  unsigned short vertex_indices[3];   /* vertex indices */
  vec3 vertex_normals[3];             /* vertex normals */
  float s[3];   /* texCoord s */
  float t[3];   /* texCoord t */
  unsigned char smoothing_group;
  unsigned char group_index;
} __ms3dtriangle;

/* group of triangles in the ms3d file */
typedef struct __MS3DPACKED {
  unsigned char flags;                /* editor flags again */
  char name[32];                      /* name of the mesh */
  unsigned short n_triangles;         /* number of triangles in the group */
  unsigned short *triangle_indices;   /* triangle indices */
  char material_index;                /* material index, -1 = no material */
} __ms3dmesh;

/* Material information */
typedef struct __MS3DPACKED {
  char name[32];
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 emissive;
  float shininess;      /* 0.0f - 128.0f */
  float transparency;   /* 0.0f - 1.0f */
  unsigned char mode;   /* 0, 1, 2 - unused now */
  char texture[128];
  char alphamap[128];
} __ms3dmaterial;

/* Keyframe data */
typedef struct __MS3DPACKED {
  float time;    /* time passed */
  vec3 param;    /* angles in radiance */
} __ms3dkey;

/* Joint information */
typedef struct __MS3DPACKED {
  unsigned char flags;
  char name[32];
  char parent_name[32];
  vec3 rot;
  vec3 trans;
  unsigned short n_rotkeys;
  unsigned short n_transkeys;
} __ms3djoint;


/* Default alignment */
#ifdef _MSC_VER
#	pragma pack(pop, packing)
#endif

#undef __MS3DPACKED


/* load meshes into ms3d */
static void
ms3d_load_meshes( ms3d *m, const __ms3dmesh *o_meshes,
                           const __ms3dtriangle *o_triangles,
                           const __ms3dvertex *o_vertices,
                           const vec3 *o_tangents,
                           const __ms3dmaterial *o_materials )
{
  int i, j, k, count;

  for(i = 0; i < m->n_mshs; i++) {
    m->o_mshs[i] = ms3dmesh_new((int)(o_meshes[i].n_triangles)*3);
    ms3dmesh *o_msh = m->o_mshs[i];

    o_msh->material_index = o_meshes[i].material_index;

    count = 0;

    for(j = 0; j < o_meshes[i].n_triangles; j++) {
      const __ms3dtriangle *triangle = &o_triangles[(int)o_meshes[i].triangle_indices[j]];
      /* loop thru 3 vertices of a triangle */
      for(k = 0; k < 3; k++) {
        int v_index = (int)triangle->vertex_indices[k];
        const __ms3dvertex *o_vertex = &o_vertices[v_index];
        vec2 o_texcoord;
        vec2_set(&o_texcoord, triangle->s[k], triangle->t[k]);

        memcpy(&o_msh->o_vcoords[count], &o_vertex->pos, sizeof(vec3));
        memcpy(&o_msh->o_normals[count], &triangle->vertex_normals[k], sizeof(vec3));
        memcpy(&o_msh->o_tangents[count], &o_tangents[v_index], sizeof(vec3));
        memcpy(&o_msh->o_texcoords[count], &o_texcoord, sizeof(vec2));
        o_msh->o_jnt_indices[count] = (short)o_vertex->bone_id;
        count++;
      }
    }
  }
}

/* store texture coordinates in graphic card buffers */
static void
ms3d_upload( const ms3d *m )
{
  int i;
  for(i = 0; i < m->n_mshs; i++) {
    ms3dmesh *o_msh = m->o_mshs[i];

    glBindBuffer(GL_ARRAY_BUFFER, o_msh->vbo_o_texcoords);
    glBufferData(GL_ARRAY_BUFFER,
                 o_msh->n_vertices*sizeof(vec2),
                 o_msh->o_texcoords,
                 GL_STATIC_DRAW);
  }
}

/* load static ms3d model vertices */
ms3d *
ms3d_new( const unsigned char *buf, const hashtable *texdb )
{
  int i, j;
  int chunk_size;

  ms3d* m = (ms3d *)malloc(sizeof(ms3d));

  /* get the model header information */
  const unsigned char *ptr = buf;
  __ms3dheader *header = (__ms3dheader *)ptr;
  ptr += sizeof(__ms3dheader);

  /* not a valid Milkshape3D model file, return 0 NULL */
  if(strncmp(header->id, "MS3D000000", 10) != 0)  return NULL;

  /* get the vertices information */
  unsigned short n_vertices = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  chunk_size = sizeof(__ms3dvertex)*n_vertices;
  __ms3dvertex *o_vertices = (__ms3dvertex *)malloc(chunk_size);
	memcpy(o_vertices, ptr, chunk_size);
  ptr += chunk_size;
  vec3 *o_tangents = (vec3 *)calloc(n_vertices, sizeof(vec3));

  /* get the triangles information */
  unsigned short n_triangles = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  chunk_size = sizeof(__ms3dtriangle)*n_triangles;
  __ms3dtriangle *o_triangles = (__ms3dtriangle *)malloc(chunk_size);
  memcpy(o_triangles, ptr, chunk_size);

  for(i = 0; i < n_triangles; i++) {
    __ms3dtriangle *triangle = (__ms3dtriangle *)ptr;
    int v_i0 = (int)triangle->vertex_indices[0];
    int v_i1 = (int)triangle->vertex_indices[1];
    int v_i2 = (int)triangle->vertex_indices[2];

    vec2 uv0, uv1, uv2;
    uv0.s = triangle->s[0];
    uv0.t = triangle->t[0];
    uv1.s = triangle->s[1];
    uv1.t = triangle->t[1];
    uv2.s = triangle->s[2];
    uv2.t = triangle->t[2];

    vec3 tangent;
    gmth_find_tangent(&tangent, &o_vertices[v_i0].pos,
                                &o_vertices[v_i1].pos,
                                &o_vertices[v_i2].pos,
                                &uv0, &uv1, &uv2);

    /* will be normalized later */
    vec3_self_add(&o_tangents[v_i0], &tangent);
    vec3_self_add(&o_tangents[v_i1], &tangent);
    vec3_self_add(&o_tangents[v_i2], &tangent);

    ptr += sizeof(__ms3dtriangle);
  }

  /* Normalise o_tangents */
  for(i = 0; i < n_vertices; i++) {
    vec3_norm(&o_tangents[i]);
  }

  /* get the mesh(group) information */
  unsigned short n_meshes = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  __ms3dmesh *o_meshes = (__ms3dmesh *)malloc(sizeof(__ms3dmesh)*n_meshes);
  for(i = 0; i < n_meshes; i++) {
    memcpy(&o_meshes[i], ptr, sizeof(__ms3dmesh));
    ptr += sizeof(__ms3dmesh);
    ptr -= sizeof(unsigned char);     /* material index */
    ptr -= sizeof(unsigned short *);  /* triangle indices */

    o_meshes[i].triangle_indices = (unsigned short *)malloc(sizeof(unsigned short)*o_meshes[i].n_triangles);
    for(j = 0; j < o_meshes[i].n_triangles; j++) {
      o_meshes[i].triangle_indices[j] = *(unsigned short *)ptr;
      ptr += sizeof(unsigned short);
    }
    o_meshes[i].material_index = *(unsigned char *)ptr;
    ptr += sizeof(unsigned char);
  }

  /* get the material information */
  unsigned short n_materials = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  __ms3dmaterial *o_materials = (__ms3dmaterial *)malloc(sizeof(__ms3dmaterial)*n_materials);
  m->o_skins = (ms3dskin *)malloc(sizeof(ms3dskin)*n_materials);
  for(i = 0; i < n_materials; i++) {
    memcpy(&o_materials[i], ptr, sizeof(__ms3dmaterial));
    vec4_cpy(&m->o_skins[i].ambient, &o_materials[i].ambient);
    vec4_cpy(&m->o_skins[i].diffuse, &o_materials[i].diffuse);
    vec4_cpy(&m->o_skins[i].specular, &o_materials[i].specular);
    vec4_cpy(&m->o_skins[i].emissive, &o_materials[i].emissive);
    m->o_skins[i].shininess = o_materials[i].shininess;
    m->o_skins[i].transparency = o_materials[i].transparency;
    util_get_fname(m->o_skins[i].texture, o_materials[i].texture);
    util_get_fname(m->o_skins[i].alphamap, o_materials[i].alphamap);
    ptr += sizeof(__ms3dmaterial);
  }

  /* load skeletal animation stuff */
  m->anim_fps = *(float *)ptr;
  ptr += sizeof(float);
  ptr += sizeof(float);   /* skip current time */
  int total_frames = *(int *)ptr;
  ptr += sizeof(int);
  m->frametime = 1000.0/m->anim_fps;
  m->total_time = total_frames*m->frametime;
  
  /* get the joint information */
  unsigned short n_joints = *(unsigned short *)ptr;
  ptr += sizeof(unsigned short);
  const unsigned char *jnt_ptr = ptr;

  /* temporarily used structures */
  typedef struct __jointlist {          /* joint list struct */
    short jnt_index;                    /* joint index */
    const char *name;
  } jointlist;

  m->o_joints = (ms3djoint *)malloc(sizeof(ms3djoint)*n_joints);
  jointlist *name_lists = (jointlist *)malloc(sizeof(jointlist)*n_joints);

  for(i = 0; i < n_joints; i++) {
    __ms3djoint *joint = (__ms3djoint *)jnt_ptr;
    jnt_ptr += sizeof(__ms3djoint);
    jnt_ptr += sizeof(__ms3dkey)*(joint->n_rotkeys + joint->n_transkeys);
    name_lists[i].jnt_index = i;
    name_lists[i].name = joint->name;
  }

  for(i = 0; i < n_joints; i++) {
    __ms3djoint *joint = (__ms3djoint *)ptr;
    ptr += sizeof(__ms3djoint);

    int parent_index = -1;
    if(joint->parent_name[0] != '\0') {
      for(j = 0; j < n_joints; j++) {
        if(strcmp(name_lists[j].name, joint->parent_name) == 0) {
          parent_index = name_lists[j].jnt_index;
          break;
        }
      }
    }

    ms3djoint *jnt = &m->o_joints[i];

    jnt->parent = parent_index;
    jnt->n_rotkeys = joint->n_rotkeys;
    jnt->n_transkeys = joint->n_transkeys;
    jnt->rotkeys = (ms3dkey *)malloc(sizeof(ms3dkey)*jnt->n_rotkeys);
    jnt->transkeys = (ms3dkey *)malloc(sizeof(ms3dkey)*jnt->n_transkeys);
    for(j = 0; j < jnt->n_rotkeys; j++) {
      __ms3dkey *key = (__ms3dkey *)ptr;
      ptr += sizeof(__ms3dkey);
      jnt->rotkeys[j].time = key->time*1000.0;
      vec3_cpy(&jnt->rotkeys[j].param, &key->param);
    }
    for(j = 0; j < jnt->n_transkeys; j++) {
      __ms3dkey *key = (__ms3dkey *)ptr;
      ptr += sizeof(__ms3dkey);
      jnt->transkeys[j].time = key->time*1000.0;
      vec3_cpy(&jnt->transkeys[j].param, &key->param);
    }

    /* initialize the matrices */
    mat4_identity(&jnt->mat_abs);
    mat4_identity(&jnt->mat_local);

    /* set rotation matrix of joint's mat_local */
    mat4_set_rot(&jnt->mat_local, joint->rot.x, joint->rot.y, joint->rot.z);
    mat4_set_trans(&jnt->mat_local, joint->trans.x, joint->trans.y, joint->trans.z);

    if(parent_index != -1)
      mat4_mul(&jnt->mat_abs, &m->o_joints[parent_index].mat_abs, &jnt->mat_local);
    else
      mat4_cpy(&jnt->mat_abs, &jnt->mat_local);

    /* get joint's mat_finals, this is a must */
    /* mat4_cpy(&m->mat_finals[i], &jnt->mat_abs); */
  }
  free(name_lists);

  /* setup each vertex associated with joint by joint's abs matrix */
  for(i = 0; i < n_vertices; i++) {
    __ms3dvertex *v = &o_vertices[i];

    if(v->bone_id != -1) {
      const mat4 *mat_abs = &m->o_joints[(int)v->bone_id].mat_abs;
      /* transform the vertices and tangents */
      vec3 tmp;
      mat4_inv_t_vec3(&tmp, mat_abs, &v->pos);
      mat4_inv_r_mul_vec3(&v->pos, mat_abs, &tmp);
      vec3_cpy(&tmp, &o_tangents[i]);
      mat4_inv_r_mul_vec3(&o_tangents[i], mat_abs, &tmp);
    }
  }

  /* go through the normals and transform them */
  for(i = 0; i < n_triangles; i++) {
		__ms3dtriangle *triangle = &o_triangles[i];

    for(j = 0; j < 3; j++) {
      /* get the vertex to find out the joint to transform the normal */
			__ms3dvertex *v = &o_vertices[triangle->vertex_indices[j]];	
      /* if it is not attached to a bone, don't do any transforms */
      if(v->bone_id == -1)  continue;

      /* transform the normal */
      const mat4 *mat_abs = &m->o_joints[(int)v->bone_id].mat_abs;
      vec3 tmp;
      vec3_cpy(&tmp, &triangle->vertex_normals[j]);
      mat4_inv_r_mul_vec3(&triangle->vertex_normals[j], mat_abs, &tmp);
    }
  }

  m->n_mshs = n_meshes;
  m->n_skins = n_materials;
  m->n_joints = n_joints;
  m->o_mshs = (ms3dmesh **)malloc(sizeof(ms3dmesh *)*m->n_mshs);
  ms3d_load_meshes(m, o_meshes, o_triangles, o_vertices, o_tangents, o_materials);

  free(o_meshes);
  free(o_triangles);
  free(o_vertices);
  free(o_tangents);
  free(o_materials);

  ms3d_upload(m);

  return m;
}

/* delete ms3d from memory */
void
ms3d_del( ms3d *m )
{
  if(!m) return;

  int i;
  for(i = 0; i < m->n_mshs; i++) {
    ms3dmesh *o_msh = m->o_mshs[i];
    ms3dmesh_del(o_msh);
  }
  free(m->o_mshs);
  free(m->o_skins);
  free(m->o_joints);
  free(m->anim_cmds);
  decal_del(m->o_aamsh);

  free(m);
}

/* create the ms3d animation struct in memory */
ms3danim *
ms3d_anim_new( const ms3d *model )
{
  ms3danim *ani = (ms3danim *)malloc(sizeof(ms3danim));
  ani->islooping = 1;
  ani->cur_transkeys = (unsigned short *)malloc(sizeof(unsigned short)*model->n_joints);
  ani->cur_rotkeys = (unsigned short *)malloc(sizeof(unsigned short)*model->n_joints);

  return ani;
}

/* delete the ms3d animationf struct from memory */
void
ms3d_anim_del( ms3danim *ani )
{
  if(!ani)  return;
  free(ani->cur_transkeys);
  free(ani->cur_rotkeys);
  free(ani);
}

/* restart the animation */
void
ms3d_reset_anim( ms3danim *ani, const unsigned short n_joints )
{
  int i;

  for(i = 0; i < n_joints; i++) {
    ani->cur_rotkeys[i] = 0;
    ani->cur_transkeys[i] = 0;
  }
  ani->cur_time = ani->ani_time.start;
}

/* Set animation command */
void
ms3d_set_anim( ms3danim *ani, const ms3d *m, const char cmd )
{
  int i;

  if(cmd != ani->cur_cmd) {
    ani->cur_cmd = cmd;
    for(i = 0; i < m->n_cmds; i++) {
      if(cmd == m->anim_cmds[i].cmd) {
        ani->ani_time.start = m->anim_cmds[i].start*m->frametime;
        ani->ani_time.end = m->anim_cmds[i].end*m->frametime;
        break;
      }
    }
    ms3d_reset_anim(ani, m->n_joints);
  }
}

/* calculate the animation time */
void
ms3d_calc_anim_time( ms3danim *ani, const ms3d *m, const float time_passed )
{
  /* get the animation time for evaluation */
  ani->cur_time += time_passed;

  if(ani->cur_time > ani->ani_time.end) {
    if(ani->islooping) {
      ms3d_reset_anim(ani, m->n_joints);
    }
    else
      ani->cur_time = m->total_time;
  }
}

/* load animation commands from a vector containing string(command) lines */
ms3dcmd *
ms3d_animcmd_new( unsigned short *n_cmds, const vector *strlines )
{
  ms3dcmd *cmds = (ms3dcmd *)malloc(sizeof(ms3dcmd)*strlines->size);

  int i;
  for(i = 0; i < strlines->size; i++) {
    char *cmdline = vector_at(strlines, i);
    int start, end;
    sscanf(cmdline, "%d %d %c", &start, &end, &cmds[i].cmd);
    cmds[i].start = start;
    cmds[i].end = end;
  }

  *n_cmds = strlines->size;

  return cmds;
}

/* animate the ms3d model, aim to calculate the mat_joint_finals */
void
ms3d_animate( mat4 *mat_joint_finals, ms3danim *ani, const ms3d *m )
{
  int i;

  for(i = 0; i < m->n_joints; i++) {
    ms3djoint *jnt = &m->o_joints[i];

    if(jnt->n_rotkeys == 0 && jnt->n_transkeys == 0) {
      mat4_cpy(&mat_joint_finals[i], &jnt->mat_abs);
      continue;
    }

    unsigned short frame;

    /* rotation frame */
    frame = ani->cur_rotkeys[i];
    while((frame < jnt->n_rotkeys) && (jnt->rotkeys[frame].time < ani->cur_time))
      ++frame;
    ani->cur_rotkeys[i] = frame;

    /* transform matrix */
    mat4 transform;
    mat4_identity(&transform);

    if(frame == 0) {
      mat4_set_rot(&transform, jnt->rotkeys[0].param.x,
                               jnt->rotkeys[0].param.y,
                               jnt->rotkeys[0].param.z);
    }
    else if(frame == jnt->n_rotkeys) {
      mat4_set_rot(&transform, jnt->rotkeys[frame - 1].param.x,
                               jnt->rotkeys[frame - 1].param.y,
                               jnt->rotkeys[frame - 1].param.z);
    }
    else {
      ms3dkey *curkey = &jnt->rotkeys[frame];
      ms3dkey *prekey = &jnt->rotkeys[frame - 1];

      float time_delta = curkey->time - prekey->time;
      float interp = (ani->cur_time - prekey->time)/time_delta;
      /* 3 quaternions - q_rev, q_cur, q_final for rotation */
      vec4 q_pre, q_cur, q_final;
      quat_from_eulers(&q_pre, prekey->param.x, prekey->param.y, prekey->param.z);
      quat_from_eulers(&q_cur, curkey->param.x, curkey->param.y, curkey->param.z);
      quat_slerp(&q_final, &q_pre, &q_cur, interp);

      mat4_from_quat(&transform, &q_final);
    }

    /* translation Frame */
    frame = ani->cur_transkeys[i];
    while((frame < jnt->n_transkeys) && (jnt->transkeys[frame].time < ani->cur_time))
      ++frame;
    ani->cur_transkeys[i] = frame;

    /* translation vector */
    vec3 translation;
	
    if(frame == 0) {
      vec3_cpy(&translation, &jnt->transkeys[0].param);
    }
    else if(frame == jnt->n_transkeys) {
      vec3_cpy(&translation, &jnt->transkeys[frame - 1].param);
    }
    else {
      ms3dkey *curkey = &jnt->transkeys[frame];
      ms3dkey *prekey = &jnt->transkeys[frame - 1];

      float time_delta = curkey->time - prekey->time;
      float interp = (ani->cur_time - prekey->time)/time_delta;

      translation.x =  prekey->param.x + (curkey->param.x - prekey->param.x)*interp;
      translation.y =  prekey->param.y + (curkey->param.y - prekey->param.y)*interp;
      translation.z =  prekey->param.z + (curkey->param.z - prekey->param.z)*interp;
    }

    /* final matrix */
    mat4 mat_joint_final;
    mat4_set_trans(&transform, translation.x, translation.y, translation.z);
    mat4_mul(&mat_joint_final, &jnt->mat_local, &transform);

    if(jnt->parent == -1)
      mat4_cpy(&mat_joint_finals[i], &mat_joint_final);
    else
      mat4_mul(&mat_joint_finals[i], &mat_joint_finals[jnt->parent], &mat_joint_final);
  }
}