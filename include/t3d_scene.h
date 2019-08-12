/*----- t3d_scene.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_scene_h_
#define _t3d_scene_h_

#include <pthread.h>
#include <t3d_type.h>


struct __scene {
  t3dsys *sys;
  camera *cam;
  light *lit;
  control *ctrl;
  pickbox *pickb;

  int geopatch_count;
  int unit_count;

  unsigned int batch_count;
};


/* create a scene in memory */
scene *scn_new( t3dsys *sys, camera *cam, light *lit, control *ctrl, pickbox *pickb );
/* delete scene from memory */
void scn_del( scene *scn );
/* update camera and light */
void scn_update_cam_lit( void *arg_scn );
/* update geopatch visibility */
void scn_update_geopatch( void *arg_scn );
/* update units */
void scn_update_unit( void *scn );
/* update vbo, ibos in the scene */
void scn_update_vbo_ibo( const t3dsys *sys );
/* the light pass */
void scn_light_pass( const t3dsys *sys, const light *lit, shader *glsl );
/* the lit pass */
void scn_camera_pass( const t3dsys *sys, const camera *cam, const light *lit, shader *glsl );


#endif  /* _t3d_scene_h_ */