/* -*- C++ -*- */

#ifndef __osp_mutex_h__
#define __osp_mutex_h__

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef void* osp_mutex_t;

  int osp_mutex_init(osp_mutex_t* mtx);
  int osp_mutex_destroy(osp_mutex_t* mtx);
  int osp_mutex_lock(osp_mutex_t* mtx);
  int osp_mutex_trylock(osp_mutex_t* mtx);
  int osp_mutex_unlock(osp_mutex_t* mtx);

#ifdef __cplusplus
}
#endif

#endif /* __osp_mutex_h__ */
