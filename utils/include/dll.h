/* -*- C++ -*- */

#ifndef __dll_h__
#define __dll_h__

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef void* osp_dll_t;
  typedef void* osp_sym_t;
  
  osp_dll_t osp_dlopen(const char* path);
  void osp_dlclose(osp_dll_t h);
  osp_sym_t osp_dlsym(osp_dll_t h, const char* addr);
  
#ifdef __cplusplus
}
#endif

#endif
