/* -*- C++ -*- */

#include <pthread.h>
#include <mutex.h>
#include <stdlib.h>

int osp_mutex_init(osp_mutex_t* mtx)
{
  pthread_mutexattr_t ma;
  if ( mtx==NULL ) return -1;
  
  *mtx = malloc(sizeof(pthread_mutex_t));
  if ( *mtx==NULL )
    return -1;
  
  if ( pthread_mutexattr_init(&ma) != 0 )
    return -1;
#ifdef OSP_MUTEX_HAS_SHARED
  if ( pthread_mutexattr_setpshared
       (&ma, PTHREAD_PROCESS_SHARED) != 0 )
    {
      pthread_mutexattr_destroy(&ma);
      return -1;
    }
#endif
  if ( pthread_mutex_init((pthread_mutex_t* )*mtx, &ma) != 0 )
    {
      pthread_mutexattr_destroy(&ma);
      return -1;
    }
  pthread_mutexattr_destroy(&ma);
  return 0;
}

int osp_mutex_destroy(osp_mutex_t* mtx)
{
  int code;
  if ( mtx==NULL || *mtx==NULL ) return -1;
  code = pthread_mutex_destroy((pthread_mutex_t* )*mtx);
  free(*mtx); *mtx = NULL;
  return code ? -1 : 0;
}

int osp_mutex_lock(osp_mutex_t* mtx)
{
  if ( mtx==NULL ) return -1;
  if ( pthread_mutex_lock((pthread_mutex_t* )*mtx) != 0 )
    return -1;
  return 0;
}

int osp_mutex_trylock(osp_mutex_t* mtx)
{
  if ( mtx==NULL ) return -1;
  if ( pthread_mutex_trylock((pthread_mutex_t* )*mtx) != 0 )
    return -1;
  return 0;
}

int osp_mutex_unlock(osp_mutex_t* mtx)
{
  if ( mtx==NULL ) return -1;
  if ( pthread_mutex_unlock((pthread_mutex_t* )*mtx) != 0 )
    return -1;
  return 0;
}
