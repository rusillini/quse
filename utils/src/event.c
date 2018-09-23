/* -*- C++ -*- */

#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <event.h>
#include <memory.h>
#include <string.h>

typedef struct _EventStruct
{
  pthread_cond_t cond;
  pthread_mutex_t mtx;
  int evt;
} EventStruct;


osp_event_t osp_event_create()
{
  EventStruct* event;
  pthread_mutexattr_t ma;
  
  event = (EventStruct* ) malloc(sizeof(EventStruct));
  if (event == NULL)
    return NULL;
  memset(event, 0, sizeof(EventStruct));

  pthread_mutexattr_init(&ma);
  pthread_mutexattr_setpshared(&ma, PTHREAD_PROCESS_SHARED);
  
  if ( pthread_mutex_init(&(event->mtx), &ma) != 0 ||
       pthread_cond_init(&(event->cond), NULL) != 0 )
    {
      osp_free(event);
      return NULL;
    }
  pthread_mutexattr_destroy(&ma);
  return (osp_event_t) event;
}

int osp_event_destroy(osp_event_t e)
{
  int ret;
  EventStruct* event = (EventStruct* ) e;
  if ( event==NULL )
    {
      errno = EINVAL;
      return -1;
    }
  
  if ( pthread_mutex_lock(&(event->mtx)) != 0 )
    return -1;
  ret = pthread_cond_destroy(&(event->cond));
  pthread_mutex_unlock(&(event->mtx));
  
  if ( ret != 0 )
    return -1;
  free(event);
  return 0;
}

int osp_event_signal(osp_event_t e)
{
  EventStruct* event = (EventStruct* )e;
  if ( event==NULL )
    {
      errno = EINVAL;
      return -1;
    }
  
  pthread_mutex_lock(&(event->mtx));
  event->evt = 1;
  pthread_mutex_unlock(&(event->mtx));
  if ( pthread_cond_signal(&(event->cond)) != 0 )
    return -1;
  
  return 0;
}

int osp_event_reset(osp_event_t e)
{
  EventStruct* event = (EventStruct*)e;
  if ( event==NULL )
    {
      errno = EINVAL;
      return -1;
    }
  
  if ( pthread_mutex_lock(&(event->mtx)) != 0 )
    return -1;
  event->evt = 0;
  pthread_mutex_unlock(&(event->mtx));
  
  return 0;
}

int osp_event_wait(osp_event_t e, int timeout, int reset)
{
  int64_ot tmpVal;
  struct timeval nowtime;
  struct timespec endtime;
  int ret;
  
  EventStruct* event = (EventStruct*)e;
  if ( event==NULL )
    {
      errno = EINVAL;
      return -1;
    }
  
  if ( timeout > -1 )
    {
      if ( gettimeofday(&nowtime, NULL) == -1 )
        return -1;
      tmpVal = nowtime.tv_usec + ((int64_ot)timeout) * 1000;
      endtime.tv_sec = nowtime.tv_sec + tmpVal / 1000000;
      endtime.tv_nsec = tmpVal % 1000000;
      endtime.tv_nsec *= 1000;
    }
  
  if ( pthread_mutex_lock(&(event->mtx)) != 0 )
    return -1;
  
  while ( event->evt == 0 )
    {
      if ( timeout > -1 )
        {
          if ( gettimeofday(&nowtime, NULL) == -1 )
            return -1;
          if ( endtime.tv_sec < nowtime.tv_sec )
            break;
          if ( endtime.tv_sec == nowtime.tv_sec &&
               endtime.tv_nsec < (1000 * (int64_ot)nowtime.tv_usec) )
            break;
          
           ret = pthread_cond_timedwait(&(event->cond), &(event->mtx), &endtime);
        }
      else
        {
          ret = pthread_cond_wait(&(event->cond), &(event->mtx));
        }
      
      if ( ret == ETIMEDOUT )
        break;
      if ( ret && ret!=EINTR )
        return -1;
    }
  
  ret = event->evt;
  if ( reset ) event->evt = 0;
  if ( pthread_mutex_unlock(&(event->mtx)) != 0 )
    return -1;
  
  if ( ret && !reset )
    {
      osp_event_signal(e);
    }
  return ret ? 0 : 1;
}
