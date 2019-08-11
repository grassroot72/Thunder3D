/*----- t3d_thpool.h ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#ifndef _t3d_thpool_h_
#define _t3d_thpool_h_

#include <t3d_type.h>


/*              _______________________________________________________
 *             /                                                       \
 *             |   JOB QUEUE        | job1 | job2 | job3 | job4 | ..   |
 *             |                                                       |
 *             |   thread pool      | thread1 | thread2 | ..           |
 *             \_______________________________________________________/
 *
 *    Description:       Jobs are added to the job queue. Once a thread in the pool
 *                       is idle, it is assigned with the first job from the queue(and
 *                       erased from the queue). It's each thread's job to read from
 *                       the queue serially(using lock) and executing each job
 *                       until the queue is empty.
 *
 *    Scheme:
 *
 *    thpool______                jobqueue____                      ______
 *    |           |               |           |       .----------->|_job0_| Newly added job
 *    |           |               |  head------------'             |_job1_|
 *    | jobqueue----------------->|           |                    |_job2_|
 *    |           |               |  tail------------.             |__..__|
 *    |___________|               |___________|       '----------->|_jobn_| Job for thread to take
 *
 *
 *    job0________
 *    |           |
 *    | function---->
 *    |           |
 *    |   arg------->
 *    |           |         job1________
 *    |  next-------------->|           |
 *    |___________|         |           |..
 */


enum {
  THPOOL_INVALID = -1,
  THPOOL_LOCK_FAILURE = -2,
  THPOOL_QUEUE_FULL = -3,
  THPOOL_QUEUE_STOP = -4,
  THPOOL_SHUTDOWN = -5,
  THPOOL_THREAD_FAILURE = -6,
  THPOOL_GRACEFUL = 1             /* destroy flag */
};


/* Creates a thpool object */
thpool *thpool_new( const int thread_count, const int queue_size );
/* stops and destroys a thread pool */
int thpool_del(thpool *pool, const int flags );
/* add a new task in the queue of a thread pool */
int thpool_add_job( thpool *pool, void (*function)(void *), void *arg, const int blocking );
/* start accepting job into the thead pool */
void thpool_start( thpool* pool );
/* stop accepting jobs into the thread pool */
void thpool_stop( thpool* pool );


#endif  /* _t3d_thpool_h_ */