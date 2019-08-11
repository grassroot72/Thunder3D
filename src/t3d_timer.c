/*----- t3d_timer.c ----------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <sys/time.h>
#endif
#include <t3d_math.h>
#include <t3d_timer.h>


/* mimic windows timeGetTime() */
unsigned int
tmr_gettime( void )
{
#ifdef WIN32
  return timeGetTime();
#else
  struct timeval now;
  gettimeofday(&now, NULL);

  return now.tv_usec/1000;
#endif
}

/* calculate the time passed, in milliseconds */
float
tmr_time_passed( vec2 *timer )
{
  float time_passed;

  timer->end = (float)tmr_gettime();
  time_passed = timer->end - timer->start;
  timer->start = timer->end;

  return time_passed;
}