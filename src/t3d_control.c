/*----- t3d_control.c --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <t3d_ogl.h>
#include <t3d_math.h>
#include <t3d_object.h>
#include <t3d_camera.h>
#include <t3d_light.h>
#include <t3d_pick.h>
#include <t3d_control.h>


struct __keyboard {
  int key;
  int action;
  int mods;
};

struct __mouse {
  int button;
  int action;
  int mods;

  vec2 pre;     /* mouse previous position, in screen coordinates */
  vec2 cur;     /* mouse current position, in screen coordinates */

  double wheel_x;
  double wheel_y;
};


/* create control struct in memory */
control *
ctrl_new( void )
{
  control *c = (control *)malloc(sizeof(control));
  c->kb = (keyboard *)malloc(sizeof(keyboard));
  c->ms = (mouse *)malloc(sizeof(mouse));

  c->kb->key = -1;
  c->kb->action = -1;

  c->ms->button = -1;
  c->ms->action = -1;
  c->ms->wheel_x = 0.0;
  c->ms->wheel_y = 0.0;

  c->quit = 0;

  return c;
}

/* delete control struct from memory */
void
ctrl_del( control *c )
{
  if(!c)  return;
  free(c->kb);
  free(c->ms);
  free(c);
}

/* update keys */
void
ctrl_key_cb( GLFWwindow* window, int key, int scancode, int action, int mods )
{
  control *c = (control *)glfwGetWindowUserPointer(window);

  if(key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GL_TRUE);
    c->quit = 1;
    return;
  }

  if(key == GLFW_KEY_T) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    return;
  }

  if(key == GLFW_KEY_Y) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return;
  }

  keyboard *kb = c->kb;
  kb->key = key;
  kb->action = action;
  kb->mods = mods;
}

/* update mouse buttons */
void
ctrl_mouse_button_cb( GLFWwindow* window, int button, int action, int mods )
{
  control *c = (control *)glfwGetWindowUserPointer(window);

  mouse *ms = c->ms;
  ms->button = button;
  ms->action = action;
  ms->mods = mods;
}

/* update cursor position */
void
ctrl_mouse_pos_cb( GLFWwindow* window, double x, double y )
{
  control *c = (control *)glfwGetWindowUserPointer(window);

  mouse *ms = c->ms;
  ms->cur.x = (float)x;
  ms->cur.y = (float)y;
}

/* update mouse wheel */
void
ctrl_mouse_wheel_cb( GLFWwindow* window, double x, double y )
{
  control *c = (control *)glfwGetWindowUserPointer(window);

  mouse *ms = c->ms;
  ms->wheel_x = x;
  ms->wheel_y = y;
}

/* process key events */
void
ctrl_process_key( const control *c, camera *cam, light *lit )
{
  keyboard *kb = c->kb;

  if(kb->action == GLFW_RELEASE) {
    kb->key = -1;
    return;
  }

  float dist, degree;

  /* keys control part */
  switch(kb->key) {
    case GLFW_KEY_Z:
      /* zoom + */
      if(cam->pos.y > cam->min_y) {
        dist = -cam->zoom_speed*cam->time;
        obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ZOOM);
      }
      break;

    case GLFW_KEY_X:
      /* zoom - */
      if(cam->pos.y < cam->max_y) {
        dist = cam->zoom_speed*cam->time;
        obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ZOOM);
      }
      break;

    case GLFW_KEY_W:
      /* move forward */
      if(cam->pos.x < cam->max_x) {
        dist = -cam->move_speed*cam->time*2.0;
        obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ON_GROUND);
        obj_move(&lit->pos, &lit->lookat, &cam->f, dist, OBJ_ON_GROUND);
      }
      break;

    case GLFW_KEY_S:
      /* move backward */
      if(cam->pos.x > cam->min_x) {
        dist = cam->move_speed*cam->time*2.0;
        obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ON_GROUND);
        obj_move(&lit->pos, &lit->lookat, &cam->f, dist, OBJ_ON_GROUND);
      }
      break;

    case GLFW_KEY_A:
      if(kb->mods == GLFW_MOD_CONTROL) {
        /* straft left */
        degree = -cam->strafe_speed*cam->time;
        obj_rot_by(&cam->pos, &cam->lookat, &cam->u_ref, degree);
      }
      else {
        /* move left */
        if(cam->pos.z > cam->min_z) {
          dist = -cam->move_speed*cam->time*2.0;
          obj_move(&cam->pos, &cam->lookat, &cam->r, dist, OBJ_ON_GROUND);
          obj_move(&lit->pos, &lit->lookat, &cam->r, dist, OBJ_ON_GROUND);
        }
      }
      break;

    case GLFW_KEY_D:
      if(kb->mods == GLFW_MOD_CONTROL) {
        /* straft right */
        degree = cam->strafe_speed*cam->time;
        obj_rot_by(&cam->pos, &cam->lookat, &cam->u_ref, degree);
      }
      else {
        /* move right */
        if(cam->pos.z < cam->max_z) {
          dist = cam->move_speed*cam->time*2.0;
          obj_move(&cam->pos, &cam->lookat, &cam->r, dist, OBJ_ON_GROUND);
          obj_move(&lit->pos, &lit->lookat, &cam->r, dist, OBJ_ON_GROUND);
        }
      }
      break;

    case GLFW_KEY_E:
      if(cam->mode == CAM_FPS_MODE) {    /* 1st person camera only */
        /* head rotate CW 
        degree = -cam->rot_speed*cam->time;
        obj_rot(&cam->lookat, &cam->pos, &cam->u, degree); */
      }
      break;

    case GLFW_KEY_Q:
      if(cam->mode == CAM_FPS_MODE) {    /* 1st person camera only */
        /* head rotate CCW 
        degree = cam->rot_speed*cam->time;
        obj_rot(&cam->lookat, &cam->pos, &cam->u, degree); */
      }
      break;

    case GLFW_KEY_UP:
      /* 3rd person camera */
      if(cam->mode == CAM_TPS_MODE && cam->pos.y < cam->max_y) {
        /* rotate about target(lookat) point around right-axis 
        degree = -cam->strafe_speed*cam->time;
        obj_rot_by(&cam->pos, &cam->lookat, &cam->r, degree); */
      }
      break;

    case GLFW_KEY_DOWN:
      /* 3rd person camera */
      if(cam->mode == CAM_TPS_MODE && cam->pos.y > cam->min_y) {
        /* rotate about target(lookat) point around right-axis 
        degree = cam->strafe_speed*cam->time;
        obj_rot_by(&cam->pos, &cam->lookat, &cam->r, degree); */
      }
      break;

    case GLFW_KEY_LEFT:
      if(cam->mode == CAM_FLY_MODE)     /* only allow in FLY mode */
        /* rolling CCW */
        /* ??? no clue yet */
      break;

    case GLFW_KEY_RIGHT:
      if(cam->mode == CAM_FLY_MODE)     /* only allow in FLY mode */
        /* rolling CW */
        /* ??? no clue yet */
      break;
  }
}

/* process mouse button and cursor events */
void
ctrl_process_mouse( const control *c, camera *cam, light *lit, pickbox *pickb )
{
  mouse *ms = c->ms;

  float dist;

  if((int)ms->cur.x <= 0) {
    /* move left */
    if(cam->pos.z > cam->min_z) {
      dist = -cam->move_speed*cam->time*2.0;
      obj_move(&cam->pos, &cam->lookat, &cam->r, dist, OBJ_ON_GROUND);
      obj_move(&lit->pos, &lit->lookat, &cam->r, dist, OBJ_ON_GROUND);
    }
    return;
  }

  if((int)ms->cur.x >= cam->screen_w - 1) {
    /* move right */
    if(cam->pos.z < cam->max_z) {
      dist = cam->move_speed*cam->time*2.0;
      obj_move(&cam->pos, &cam->lookat, &cam->r, dist, OBJ_ON_GROUND);
      obj_move(&lit->pos, &lit->lookat, &cam->r, dist, OBJ_ON_GROUND);
    }
    return;
  }

  if((int)ms->cur.y <= 0) {
    /* move forward */
    if(cam->pos.x < cam->max_x) {
      dist = -cam->move_speed*cam->time*2.0;
      obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ON_GROUND);
      obj_move(&lit->pos, &lit->lookat, &cam->f, dist, OBJ_ON_GROUND);
    }
    return;
  }

  if((int)ms->cur.y >= cam->screen_h - 1) {
    /* move backward */
    if(cam->pos.x > cam->min_x) {
      dist = cam->move_speed*cam->time*2.0;
      obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ON_GROUND);
      obj_move(&lit->pos, &lit->lookat, &cam->f, dist, OBJ_ON_GROUND);
    }
    return;
  }

  /* left mouse buttion was pressed */
  if(ms->button == GLFW_MOUSE_BUTTON_LEFT) {
    /* pressed? */
    if(ms->action == GLFW_PRESS) {
      pickb->status = PICK_PICKING;
      ms->pre.x = ms->cur.x;
      ms->pre.y = ms->cur.y;
    }

    /* released? */
    if(ms->action == GLFW_RELEASE) {
      pickb->status = PICK_LEFT_PICKED;
      if(fabs(ms->cur.x - ms->pre.x) < 2.0 && fabs(ms->cur.y - ms->pre.y) < 2.0) {
        pickb->type = PICK_SINGLE;
        ms->pre.x = ms->cur.x;
        ms->pre.y = ms->cur.y;
      }
      else
        pickb->type = PICK_MULTI;
    }
  }

  /* right mouse buttion was pressed */
  if(ms->button == GLFW_MOUSE_BUTTON_RIGHT) {
    /* pressed? */
    if(ms->action == GLFW_PRESS) {
      pickb->status = PICK_RIGHT_PICKED;
      pickb->v0.x = ms->cur.x;
      pickb->v0.y = ms->cur.y;
    }
  }

  /* mouse is moving while picking */
  if(pickb->status == PICK_PICKING) {
    vec2 bmin, bmax;
    vec2_cpy(&bmin, &ms->cur);
    vec2_cpy(&bmax, &ms->cur);
    vec2_take_min(&bmin, &ms->pre);
    vec2_take_max(&bmax, &ms->pre);
    /* convert to NDC coordinate */
    vec2_set(&pickb->v0, bmin.x, (float)cam->screen_h - bmin.y);
    vec2_set(&pickb->v1, bmax.x, (float)cam->screen_h - bmax.y);
  }

  if(ms->wheel_y > 0.0) {
    /* zoom + */
    if(cam->pos.y > cam->min_y) {
      dist = -cam->zoom_speed*cam->time;
      obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ZOOM);
    }

    ms->wheel_y = 0.0;
  }

  if(ms->wheel_y < 0.0) {
    /* zoom - */
    if(cam->pos.y < cam->max_y) {
      dist = cam->zoom_speed*cam->time;
      obj_move(&cam->pos, &cam->lookat, &cam->f, dist, OBJ_ZOOM);
    }

    ms->wheel_y = 0.0;
  }

  /* clear the button */
  ms->button = -1;
}