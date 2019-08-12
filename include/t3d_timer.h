/*----- t3d_timer.h ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_timer_h_
#define _t3d_timer_h_

#include <t3d_type.h>


/* mimic windows timeGetTime() */
unsigned int tmr_gettime( void );
/* calculate the time passed, in milliseconds */
float tmr_time_passed( vec2 *timer );


#endif   /* _t3d_timer_h_ */