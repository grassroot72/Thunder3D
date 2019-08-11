/*----- t3d_control.h --------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_control_h_
#define _t3d_control_h_

#include <GL/glfw3.h>
#include <t3d_type.h>


typedef struct __keyboard keyboard;
typedef struct __mouse    mouse;

struct __control {
  keyboard *kb;
  mouse *ms;

  int quit;
};

/* create control struct in memory */
control *ctrl_new( void );
/* delete control struct from memory */
void ctrl_del( control *c );
/* update keys */
void ctrl_key_cb( GLFWwindow* window, int key, int scancode, int action, int mods );
/* update mouse buttons */
void ctrl_mouse_button_cb( GLFWwindow* window, int button, int action, int mods );
/* update cursor position */
void ctrl_mouse_pos_cb( GLFWwindow* window, double x, double y );
/* update mouse wheel */
void ctrl_mouse_wheel_cb( GLFWwindow* window, double x, double y );
/* process key events */
void ctrl_process_key( const control *c, camera *cam, light *lit );
/* process mouse button and cursor events */
void ctrl_process_mouse( const control *c, camera *cam, light *lit, pickbox *pickb );


#endif  /* _t3d_control_h_ */