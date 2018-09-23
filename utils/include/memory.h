#ifndef __osp_memory_h__
#define __osp_memory_h__

#include "config.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  void *osp_malloc(u_intp_ot n);
  void *osp_realloc(void* p, u_intp_ot n);
  void *osp_realloc_ex(void* p, u_intp_ot n);
  void osp_free(void* p);
  
  u_intp_ot osp_strlen(const char* s);
  char* osp_strdup(const char* s);
  intp_ot osp_strcmp(const char* s1, const char* s2);
  intp_ot osp_strncmp(const char* s1, const char* s2,
                                      intp_ot len);
  char* osp_strcat(char* s1, const char* s2);
  char* osp_strdup2(const char* s, u_intp_ot n);
  char* osp_strstr(char* s, const char* ss);
  void* osp_memdup(const void* d, u_intp_ot n);
  void* osp_memdup2(const void* d, u_intp_ot n,
                                    u_intp_ot k);
  void osp_check_mem(const char* check);
  
#ifdef __cplusplus
}
#endif

#endif /* __osp_memory_h__ */
