/*----- t3d_type.h -----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_type_h_
#define _t3d_type_h_


/* t3d math typedefs */
typedef union __ivec2 ivec2;
typedef union __ivec3 ivec3;
typedef union __ivec4 ivec4;
typedef union __vec2  vec2;
typedef union __vec3  vec3;
typedef union __vec4  vec4;
typedef union __mat2  mat2;
typedef union __mat3  mat3;
typedef union __mat4  mat4;

/* t3d generic vector struct */
typedef struct __vector vector;

/* t3d hash table struct */
typedef struct __hashtable hashtable;

/* t3d single linked list */
typedef struct __listnode listnode;
typedef struct __list     list;
typedef struct __list     *list_p;

/* t3d control struct */
typedef struct __control  control;

/* t3d pickbox struct */
typedef struct __pickbox pickbox;

/* t3d camera struct */
typedef struct __camera camera;

/* t3d light struct */
typedef struct __light light;

/* t3d frustum struct */
typedef struct __frustum frustum;

/* t3d ms3dmesh struct */
typedef struct __ms3dmesh  ms3dmesh;

/* ms3d structs */
typedef struct __ms3d      ms3d;
typedef struct __ms3dskin  ms3dskin;
typedef struct __ms3danim  ms3danim;
typedef struct __ms3dcmd   ms3dcmd;

/* t3d mesh struct */
typedef struct __mesh mesh;

/* t3d unit struct */
typedef struct __unit unit;

/* t3d geopatch, cell, patchmap struct */
typedef struct __geopatch geopatch;
typedef struct __cell     cell;
typedef struct __patchmap patchmap;

/* t3d minimap struct */
typedef struct __minimap minimap;

/* t3d decal struct */
typedef struct __decal  decal;

/* t3d axis aligned decal mesh struct */
typedef struct __aamesh aamesh;

/* t3d astar cell & node struct - path finding */
typedef struct __anode anode;

/* t3d font struct */
typedef struct __font font;

/* t3d shader struct */
typedef struct __shader shader;

/* t3d system struct */
typedef struct __t3dsys t3dsys;

/* t3d thpool struct */
typedef struct __thpool thpool;

/* t3d audio struct */
typedef struct __audiostream audiostream;

/* t3d scene struct */
typedef struct __scene scene;


#endif   /* _t3d_type_h_ */
