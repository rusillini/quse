/* -*- C++ -*- */
#ifndef __osp_environ_h__
#define __osp_environ_h__

#include "config.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  char* osp_getenv(const char* name);
  char* osp_getenv2(const char* name, u_intp_ot len);
  
  int osp_putenv(const char* env);
  int osp_putenv2(const char* name, const char* val);
  
  int osp_initenv(const char* name);
  
  char* osp_str_norm(const char* path);
  char* osp_str_subst(const char* str);
  
  char* osp_path_norm(const char* path);
  
   char* osp_modpath(const char* type,
                     const char* id,
                     const char* dir);
  
  char* osp_path_build(const char* const args[]);
  
  intp_ot osp_error_get();
  char* osp_error_fmt(intp_ot e);

  char* osp_gethostname();

#ifdef __cplusplus
}
#endif

#endif /* __osp_environ_h__ */
