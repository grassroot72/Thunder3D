/*----- main.c --------------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wchar.h>
#include <GL/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <stb_vorbis.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_shader.h>
#include <t3d_timer.h>
#include <t3d_slist.h>
#include <t3d_control.h>
#include <t3d_pick.h>
#include <t3d_object.h>
#include <t3d_camera.h>
#include <t3d_light.h>
#include <t3d_patchmap.h>
#include <t3d_minimap.h>
#include <t3d_sys.h>
#include <t3d_font.h>
#include <t3d_thpool.h>
#include <t3d_scene.h>


int
main( int argc, char* argv[] )
{
  /* create glfw window */
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: Unable to initialize GLFW\n");
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_SAMPLES, 8);
  glfwWindowHint(GLFW_REFRESH_RATE, 0);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 1);

  int screen_w = 1280;
  int screen_h = 720;

  GLFWwindow* window;
  /* to use fullsceen with glfw3, fill the monitor with glfwGetPrimaryMonitor() */
  window = glfwCreateWindow(screen_w, screen_h, "T3D Engine Demo", glfwGetPrimaryMonitor(), NULL);
  /* Open OpenGL window */
  //window = glfwCreateWindow(screen_w, screen_h, "T3D Engine Demo", NULL, NULL);
  if(!window) {
    fprintf(stderr, "ERROR: Unable to create the OpenGL context and associated window\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetMouseButtonCallback(window, ctrl_mouse_button_cb);
  glfwSetCursorPosCallback(window, ctrl_mouse_pos_cb);
  glfwSetScrollCallback(window, ctrl_mouse_wheel_cb);
  glfwSetKeyCallback(window, ctrl_key_cb);
  /* Enable vsync */
  glfwSwapInterval(1);

  glfwMakeContextCurrent(window);


  /* OpenAL context */
  const ALCchar *default_device_name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
  ALCdevice *device;
  ALCcontext *context;

  device = alcOpenDevice(default_device_name);
  context = alcCreateContext(device, NULL);
  alcMakeContextCurrent(context);

  ALuint source;
  ALuint buffer;
  alGenBuffers(1, &buffer);
  alGenSources(1, &source); 

  int n_channels, datasize, frequency, format;
  short *pcm;
  datasize = stb_vorbis_decode_filename("music/12mornings.ogg", &n_channels, &frequency, &pcm);

  if(n_channels == 2)
    format = AL_FORMAT_STEREO16;
  else
    format = AL_FORMAT_MONO16;

  alBufferData(buffer, format, pcm, datasize*sizeof(ALshort), frequency);

  free(pcm);

  ALfloat SourcePos[] = {0.0, 0.0, 0.0};                   /* Position of the source sound */
  ALfloat SourceVel[] = {0.0, 0.0, 0.0};                   /* velocity of the source sound */
  ALfloat ListenerPos[] = {0.0, 0.0, 0.0};                 /* Position of the listener */
  ALfloat ListenerVel[] = {0.0, 0.0, 0.0};                 /* velocity of the listener */
  ALfloat ListenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0}; /* orientation of the listener */

  /* listener */                                                                           
  alListenerfv(AL_POSITION, ListenerPos);                  /* set position of the listener */
  alListenerfv(AL_VELOCITY, ListenerVel);                  /* set velocity of the listener */
  alListenerfv(AL_ORIENTATION, ListenerOri);               /* set orientation of the listener */
   
  //Source
  alSourcei(source, AL_BUFFER, buffer);                    /* link the buffer to the source */
  alSourcef(source, AL_PITCH, 1.0);                        /* set the pitch of the source */
  alSourcef(source, AL_GAIN, 1.0);                         /* set the gain of the source */
  alSourcefv(source, AL_POSITION, SourcePos);              /* set the position of the source */
  alSourcefv(source, AL_VELOCITY, SourceVel);              /* set the velocity of the source */
  alSourcei(source, AL_LOOPING, AL_TRUE);                  /* set if source is looping sound */

  /* play the OpenAL sound */
  alSourcePlay(source);


  /* initialize opengl functions */
  if(!ogl_init()) {
    fprintf(stderr, "ERROR: Cannot resolve OpenGL functions\n");
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  /* create a new t3d system */
  t3dsys *sys = sys_new();

  /* create minimap */
  minimap *mmap = minimap_new("terrain.png", screen_w, screen_h, sys->texdb);

  /* create font */
  font *fnt = fnt_new("fonts/FreeSans.ttf", 20, 512, 512, 32, 96, screen_w, screen_h);

  vec3 lookat;
  vec3 cam_pos;
  vec3 lit_pos;
  vec3_set(&cam_pos, 8.0, 658.0, 498.0);
  vec3_set(&lit_pos, 318.0, 688.0, 18.0);
  vec3_set(&lookat, 318.0, 38.0, 498.0);

  /* create a 3d camera */
  camera *cam_3d = cam_new(CAM_TPS_MODE, &cam_pos, &lookat, screen_w, screen_h, sys->pchmap);
  cam_set_view(cam_3d);

  /* create lights */
  light *lit0 = lit_new(2048, &lit_pos, &lookat);
  lit_set_view(lit0);

  /* create control */
  control *ctrl = ctrl_new();
  /* hook the control struct here */
  glfwSetWindowUserPointer(window, ctrl);
  /* create the pick box */
  pickbox *pickb = pick_new(screen_w, screen_h);

  /* create shaders */
  shader *glsl = shd_new();
  glsl->prog_normal_pass = shd_load("shaders/normal_pass.vs", "shaders/normal_pass.fs");
  glsl->prog_light_pass = shd_load("shaders/light_pass.vs", "shaders/light_pass.fs");
  glsl->prog_font = shd_load("shaders/font.vs", "shaders/font.fs");
  glsl->prog_box2d = shd_load("shaders/box2d.vs", "shaders/box2d.fs");
  glsl->prog_aamesh = shd_load("shaders/aamesh.vs", "shaders/aamesh.fs");
  glsl->prog_minimap = shd_load("shaders/minimap.vs", "shaders/minimap.fs");


  scene *scn = scn_new(sys, cam_3d, lit0, ctrl, pickb);
  thpool *th_pool= thpool_new(4, 5000);

  while(!glfwWindowShouldClose(window)) {
    if(scn->batch_count == 3) {
      scn->batch_count = 0;
      /* draw scene */
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      scn_update_vbo_ibo(sys);
      scn_light_pass(sys, lit0, glsl);
      scn_camera_pass(sys, cam_3d, lit0, glsl);

      /* draw GUI text */
      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      glDepthMask(GL_FALSE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      if(pickb->status == PICK_PICKING)
        pick_draw_box2d(pickb, &pickb->v0, &pickb->v1, &sys->colors[CLR_GREEN], glsl);

      pick_check_left_picked(pickb, sys->units, cam_3d);
      pick_check_right_picked(pickb, sys->pchmap, sys->units, cam_3d);

      /* system timer, should be placed in main thread */
      sys->time_passed = tmr_time_passed(&sys->timer);
      cam_3d->time = sys->time_passed;
      lit0->time = sys->time_passed;

      wchar_t str[32];
      swprintf(str, 32, L"FPS: %.2f", 1000.0/sys->time_passed);  /* FPS */
      fnt_print(fnt, str,
                (float)cam_3d->screen_w - 162.0, (float)cam_3d->screen_h - 102.0,
                &sys->colors[CLR_YELLOW], glsl);

      fnt_print(fnt, L"Thunder3D Engine",
                (float)cam_3d->screen_w - 162.0, (float)cam_3d->screen_h - 42.0,
                &sys->colors[CLR_CYAN], glsl);
      fnt_print(fnt, L"Technology Demo",
                (float)cam_3d->screen_w - 162.0, (float)cam_3d->screen_h - 62.0,
                &sys->colors[CLR_RED], glsl);

      fnt_print(fnt, L"Cross System Support (Linux/Windows)", 30.0, 230.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"OpenGL GLSL Rendering", 30.0, 250.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Task Based Multithread Support", 30.0, 270.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Scalable Shadowmapping", 30.0, 290.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Normalmaping", 30.0, 310.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Heightmap Based Patchmap", 30.0, 330.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Unicode Font support", 30.0, 350.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Raycast Picking", 30.0, 370.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Central Texture Management", 30.0, 390.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Central Model Management", 30.0, 410.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Skeletal Animation", 30.0, 430.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Simple Mesh Decal", 30.0, 450.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"OpenAL 3D Sound Support", 30.0, 470.0, &sys->colors[CLR_GREEN], glsl);
      fnt_print(fnt, L"Vorbis Format Support", 30.0, 490.0, &sys->colors[CLR_GREEN], glsl);

      fnt_print(fnt, L"[ Zoom ] - Mouse wheel", 30.0, 520.0, &sys->colors[CLR_WHITE], glsl);
      fnt_print(fnt, L"[ Move Around ] - Mouse to the borders, or A, D, W, S", 30.0, 540.0, &sys->colors[CLR_WHITE], glsl);

      /* draw the minimap at lower-left corner */
      minimap_draw(mmap, glsl);

      glDisable(GL_BLEND);
      glDepthMask(GL_TRUE);

      /* display and process events through callbacks */
      glfwSwapBuffers(window);
      glfwPollEvents();
      if(ctrl->quit) {
        glfwWindowShouldClose(window);
      }
      else {
        /* add jobs to thread pool */
        int i;
        thpool_add_job(th_pool, &scn_update_cam_lit, scn, 1);
        for(i = 0; i < sys->pchmap->n_total_pchs; i++) {
          thpool_add_job(th_pool, &scn_update_geopatch, scn, 1);
        }
        for(i = 0; i < sys->units->size; i++) {
          thpool_add_job(th_pool, &scn_update_unit, scn, 1);
        }
      }
    }
  }

  thpool_del(th_pool, THPOOL_GRACEFUL);
  cam_del(cam_3d);
  lit_del(lit0);
  fnt_del(fnt);
  minimap_del(mmap);
  sys_del(sys);
  ctrl_del(ctrl);
  pick_del(pickb);
  shd_del(glsl);

  scn_del(scn);

  /* exit OpenAL context */
  alDeleteSources(1, &source);
  alDeleteBuffers(1, &buffer);
  device = alcGetContextsDevice(context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);


  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
