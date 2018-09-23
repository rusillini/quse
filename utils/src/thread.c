/* -*- C++ -*- */

#include <thread.h>

osp_thread_t osp_thread_self(void)
{
  return (osp_thread_t) pthread_self();
}

int osp_thread_equal(osp_thread_t t1, osp_thread_t t2)
{
  if (t1 == NULL || t2 == NULL)
    return -1;
  if (pthread_equal((pthread_t)t1, (pthread_t)t2) != 0)
    return 0;
  return 1;
}

typedef void* (*tmp_prthread_routine_t)(void *);

int osp_thread_create(osp_thread_t *thread,
                      osp_thread_func start_routine,
                      void* arg, int priority, int flags)
{
  struct sched_param param;
  pthread_attr_t attr;
  if (pthread_attr_init(&attr) != 0)
    return -1;
  if (pthread_attr_getschedparam(&attr, &param) != 0)
    {
      pthread_attr_destroy(&attr);
      return -1;
    }
  switch ( priority )
    {
    case OSP_THR_LOW:
    case OSP_THR_MIDLOW:
    case OSP_THR_NORMAL:
    case OSP_THR_MIDHIGH:
    case OSP_THR_HIGH:
      break;
    default:
      priority = OSP_THR_NORMAL;
    }
  
  param.sched_priority = priority;
  if (pthread_attr_setschedparam(&attr, &param) != 0)
    {
      pthread_attr_destroy(&attr);
      return -1;
    }
  if ( flags & OSP_THR_DETACHED )
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  else
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
  
  return pthread_create((pthread_t*)thread, &attr,
                        (tmp_prthread_routine_t)start_routine, arg)
    == 0 ? 0 : -1;
}

int osp_thread_join(osp_thread_t thread, intp_ot* retval)
{
  return pthread_join((pthread_t)thread, (void**)retval) == 0 ? 0 : -1;
}

void osp_thread_exit(intp_ot value)
{
  pthread_exit((void*)value);
}

/************************************************************/
static pthread_mutex_t g_sigproc_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_t g_sigproc_thread = (pthread_t)0;
static osp_exit_proc_t g_sigproc_proc = NULL;
static sigset_t g_sigproc_sigs;

static void* sig_handler(void *p)
{
  int signo;
  
  pthread_mutex_lock(&g_sigproc_mtx);
  if ( g_sigproc_proc == NULL )
    {
      pthread_mutex_unlock(&g_sigproc_mtx);
      pthread_exit(NULL);
    }
  pthread_mutex_unlock(&g_sigproc_mtx);
  
  while (1)
    {
      if ( sigwait(&g_sigproc_sigs, &signo) < 0 )
        pthread_exit(NULL);
      switch ( signo )
        {
        case SIGINT:
#ifdef SIGHUP
        case SIGHUP:
#endif
#ifdef SIGQUIT
        case SIGQUIT:
#endif
#ifdef SIGTERM
        case SIGTERM:
#endif
          g_sigproc_proc();
        }
    }
  return NULL;
}


int osp_prepare_main(osp_exit_proc_t proc)
{
  pthread_attr_t thr_attr;
  
  if ( osp_prepare_thread() != 0 )
    return -1;
  
  if ( proc==NULL )
    return -1;
  
  pthread_mutex_lock(&g_sigproc_mtx);
  if ( g_sigproc_proc == NULL )
    {
      pthread_attr_init(&thr_attr);
      pthread_attr_setdetachstate(&thr_attr, PTHREAD_CREATE_DETACHED);
      pthread_attr_setscope(&thr_attr, PTHREAD_SCOPE_SYSTEM);
      if ( pthread_create
           (&g_sigproc_thread, &thr_attr, sig_handler, NULL) != 0 )
        {
          pthread_mutex_unlock(&g_sigproc_mtx);
          return -1;
        }
      g_sigproc_proc = proc;
    }
  pthread_mutex_unlock(&g_sigproc_mtx);
  return 0;
}

int osp_prepare_thread(void)
{
  pthread_mutex_lock(&g_sigproc_mtx);
  
  if ( g_sigproc_proc == NULL )
    {
      sigemptyset(&g_sigproc_sigs);
      sigaddset(&g_sigproc_sigs, SIGINT);
#ifdef SIGHUP
      sigaddset(&g_sigproc_sigs, SIGHUP);
#endif
#ifdef SIGPIPE
      sigaddset(&g_sigproc_sigs, SIGPIPE);
#endif
#ifdef SIGPOLL
      sigaddset(&g_sigproc_sigs, SIGPOLL);
#endif
#ifdef SIGQUIT
      sigaddset(&g_sigproc_sigs, SIGQUIT);
#endif
#ifdef SIGTERM
      sigaddset(&g_sigproc_sigs, SIGTERM);
#endif
#ifdef SIGUSR1
      sigaddset(&g_sigproc_sigs, SIGUSR1);
#endif
#ifdef SIGUSR2
      sigaddset(&g_sigproc_sigs, SIGUSR2);
#endif
/* #ifdef SIGCHLD
      sigaddset(&g_sigproc_sigs, SIGCHLD);
   #endif */
#ifdef SIGALRM
      sigaddset(&g_sigproc_sigs, SIGALRM);
#endif
#ifdef SIGPWR
      sigaddset(&g_sigproc_sigs, SIGPWR);
#endif
#ifdef SIGXCPU
      sigaddset(&g_sigproc_sigs, SIGXCPU);
#endif
#ifdef SIGXFSZ
      sigaddset(&g_sigproc_sigs, SIGXFSZ);
#endif
#ifdef SIGVTALRM
      sigaddset(&g_sigproc_sigs, SIGVTALRM);
#endif
#ifdef SIGWINCH
      sigaddset(&g_sigproc_sigs, SIGWINCH);
#endif
#ifdef SIGIO
      sigaddset(&g_sigproc_sigs, SIGIO);
#endif
    }
  
  if ( pthread_sigmask(SIG_BLOCK, &g_sigproc_sigs, NULL) != 0 ) {
     pthread_mutex_unlock(&g_sigproc_mtx);
     return -1;
  }
  pthread_mutex_unlock(&g_sigproc_mtx);
  return 0;
}
