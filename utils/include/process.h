/* -*- C++ -*- */

#ifndef __process_h__
#define __process_h__

#include <files.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef intp_ot osp_pid_t;
  
#define OSP_PID_BAD      ((osp_pid_t)0)

#define OSP_PID_FINISHED ((osp_pid_t)~0)
  
  osp_pid_t osp_spawnl(const char* const args[], intp_ot* pret);
  
  osp_pid_t osp_spawnl_ex(const char* const args[],
                osp_file_t fds[], intp_ot* ret);
  
  intp_ot osp_testpid(osp_pid_t pid, intp_ot* pret);
  
  intp_ot osp_waitpid(osp_pid_t pid, intp_ot* pret, intp_ot wait);
  
  intp_ot osp_termpid(osp_pid_t pid);

  intp_ot osp_huppid(osp_pid_t pid);
  
  void osp_closepid(osp_pid_t pid);
  
  osp_pid_t osp_getpid(void);
  
  intp_ot osp_runprc(const char* const args[], u_intp_ot wait, intp_ot* pret);
  
  int osp_daemonize(const char* wpath, int fdclose);
  
  void osp_sleep(int msec, int alert);
  
#ifdef __cplusplus
}
#endif

#endif /* __process_h__ */
