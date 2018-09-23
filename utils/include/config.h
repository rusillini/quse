/* -*- C++ -*- */
#ifndef __osp_config_h__
#define __osp_config_h__

#include "cf-all-gcc.h"

#ifdef __cplusplus
extern "C" {
#endif
  typedef void* (*osp_malloc_t)(u_intp_ot n);
  typedef void  (*osp_free_t)(void*);
  typedef void* osp_void_ptr;
  typedef u_int64_ot osp_time_t;
#ifdef __cplusplus
}
#endif

#endif /* _osp_config_h__ */
