/* -*- C++ -*- */

#ifndef __thread_h__
#define __thread_h__

#include <config.h>
#include <pthread.h>
#include <signal.h>

#ifndef _REENTRANT
#define _REENTRANT 1
#endif
#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS 1
#endif

#define OSP_THR_LOW     10
#define OSP_THR_MIDLOW   5
#define OSP_THR_NORMAL   0
#define OSP_THR_MIDHIGH -5
#define OSP_THR_HIGH   -10

#ifdef __cplusplus
extern "C" {
#endif

#define OSP_THR_DETACHED 1
    
  typedef void* osp_thread_t;
  
  typedef intp_ot (*osp_thread_func)(void* arg);
  
  osp_thread_t osp_thread_self(void);
  
  int osp_thread_equal(osp_thread_t t1, osp_thread_t t2);
  
  int osp_thread_create(osp_thread_t *thread,
                        osp_thread_func start_routine,
                        void* arg, int priority, int flags);
  
  int osp_thread_join(osp_thread_t thread, intp_ot* retval);

  void osp_thread_exit(intp_ot value);
  
  typedef void (*osp_exit_proc_t)(void);
  
  int osp_prepare_main(osp_exit_proc_t proc);
  
  int osp_prepare_thread(void);

#ifdef __cplusplus
}
#endif

#endif /* __thread_h__ */
