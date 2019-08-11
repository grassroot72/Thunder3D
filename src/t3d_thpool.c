/*----- t3d_thpool.c ---------------------------------------------------------+
    Developped by: Edward Lei
    Revised date: 18th Jan, 2014
 +----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <t3d_thpool.h>


enum {
  SHUTDOWN_NOW = 1,
  SHUTDOWN_GRACEFUL = 2
};


typedef struct __thpool_job {
  void (*func)(void *);
  void *arg;

  int blocking;
} thpool_job;


struct __thpool {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  thpool_job *queue;
  int thread_count;
  int queue_size;
  int head;
  int tail;
  int count;
  int stop;
  int shutdown;
  int started;
};


static void *
thpool_worker_thread( void *th_pool )
{
  thpool *pool = (thpool *)th_pool;
  thpool_job *job;

  for(;;) {
    /* lock must be taken to wait on conditional variable */
    pthread_mutex_lock(&pool->lock);

    /* wait on condition variable, check for spurious wakeups.
       when returning from pthread_cond_wait(), we own the lock. */
    while(pool->count == 0 && !pool->shutdown) {
      pthread_cond_wait(&pool->notify, &pool->lock);
    }

    if(pool->shutdown == SHUTDOWN_NOW ||
      (pool->shutdown == SHUTDOWN_GRACEFUL && pool->count == 0)) {
      break;
    }

    /* grab the job */
    job = &pool->queue[pool->head];
    pool->head += 1;
    pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
    pool->count -= 1;

    /* get to work */
    if(job->blocking) {
      (*(job->func))(job->arg);
      /* unlock */
      pthread_mutex_unlock(&pool->lock);
    }
		else {
      /* unlock */
      pthread_mutex_unlock(&pool->lock);
      (*(job->func))(job->arg);
    }
  }

  pool->started--;

  pthread_mutex_unlock(&pool->lock);
  pthread_exit(NULL);
  return NULL;
}

static int
thpool_free( thpool *pool )
{
  if(pool == NULL || pool->started > 0) {
    return THPOOL_INVALID;
  }

  /* did we manage to allocate ? */
  if(pool->threads) {
    free(pool->threads);
    free(pool->queue);
 
    /* because we allocate pool->threads after initializing the
       mutex and condition variable, we're sure they're
       initialized. let's lock the mutex just in case. */
    pthread_mutex_lock(&pool->lock);
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
  }

  free(pool);    
  return 0;
}

/* Creates a thpool object */
thpool *
thpool_new( const int n_threads, const int queue_size )
{
  thpool *pool;
  int i;

  pool = (thpool *)malloc(sizeof(thpool));

  /* initialize */
  pool->thread_count = 0;
  pool->queue_size = queue_size;
  pool->head = pool->tail = pool->count = 0;
  pool->stop = 0;
  pool->shutdown = pool->started = 0;

  /* allocate thread and task queue */
  pool->threads = (pthread_t *)malloc(sizeof(pthread_t)*n_threads);
  pool->queue = (thpool_job *)malloc(sizeof(thpool_job)*queue_size);

  /* initialize mutex and conditional variable first */
  if((pthread_mutex_init(&pool->lock, NULL) != 0) ||
     (pthread_cond_init(&pool->notify, NULL) != 0) ||
     (pool->threads == NULL) ||
     (pool->queue == NULL)) {
    thpool_free(pool);
    return NULL;
  }

  /* start worker threads */
  for(i = 0; i < n_threads; i++) {
    if(pthread_create(&(pool->threads[i]), NULL, thpool_worker_thread, (void *)pool) != 0) {
      thpool_del(pool, 0);
      return NULL;
    }
    pool->thread_count++;
    pool->started++;
  }

  return pool;
}

/* stops and destroys a thread pool */
int
thpool_del( thpool *pool, const int flags )
{
  int i, ret = 0;

  if(pool == NULL) return THPOOL_INVALID;

  if(pthread_mutex_lock(&pool->lock) != 0) {
    return THPOOL_LOCK_FAILURE;
  }

  do {
    /* already shutting down */
    if(pool->shutdown) {
      ret = THPOOL_SHUTDOWN;
      break;
    }

    pool->shutdown = (flags & THPOOL_GRACEFUL) ? SHUTDOWN_GRACEFUL : SHUTDOWN_NOW;

    /* wake up all worker threads */
    if((pthread_cond_broadcast(&pool->notify) != 0) ||
       (pthread_mutex_unlock(&pool->lock) != 0)) {
      ret = THPOOL_LOCK_FAILURE;
      break;
    }

    /* join all worker thread */
    for(i = 0; i < pool->thread_count; i++) {
      if(pthread_join(pool->threads[i], NULL) != 0) {
        ret = THPOOL_THREAD_FAILURE;
      }
    }
  } while(0);

  /* only if everything went well do we deallocate the pool */
  if(!ret) {
    thpool_free(pool);
  }

  return ret;
}

/* add a new task in the queue of a thread pool */
int
thpool_add_job( thpool *pool, void (*func)(void *), void *arg, const int blocking )
{
  int ret = 0;
  int next;
  thpool_job *job;

  if(pool == NULL || func == NULL)  return THPOOL_INVALID;

  if(pthread_mutex_lock(&pool->lock) != 0) {
    return THPOOL_LOCK_FAILURE;
  }

  next = pool->tail + 1;
  next = (next == pool->queue_size) ? 0 : next;

  do {
    /* are we full ? */
    if(pool->count == pool->queue_size) {
      ret = THPOOL_QUEUE_FULL;
      break;
    }

    /* are we shutting down ? */
    if(pool->shutdown) {
      ret = THPOOL_SHUTDOWN;
      break;
    }

    if(pool->stop) {
      ret = THPOOL_QUEUE_STOP;
      break;
    }

    /* add job to queue */
    job = &pool->queue[pool->tail];
    job->func = func;
    job->arg = arg;
    job->blocking = blocking;
    pool->tail = next;
    pool->count += 1;

    /* pthread_cond_broadcast */
    if(pthread_cond_signal(&pool->notify) != 0) {
      ret = THPOOL_LOCK_FAILURE;
      break;
    }
  } while(0);

  if(pthread_mutex_unlock(&pool->lock) != 0) {
    ret = THPOOL_LOCK_FAILURE;
  }

  return ret;
}

/* start accepting job into the thead pool */
void
thpool_start( thpool* pool )
{
  pool->stop = 0;
}

/* stop accepting jobs into the thread pool */
void
thpool_stop( thpool* pool )
{
  pool->stop = 1;
}