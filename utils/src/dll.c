/* -*- C++ -*- */

#include <dlfcn.h>
#include <dll.h>

#ifdef RTLD_LOCAL
# define DLOPEN_FLAGS (RTLD_NOW | RTLD_LOCAL)
#else
# define DLOPEN_FLAGS RTLD_NOW
#endif

osp_dll_t osp_dlopen(const char* path)
{
  void *handle = dlopen(path, DLOPEN_FLAGS);
  return (osp_dll_t)handle;
}

void osp_dlclose(osp_dll_t h)
{
  dlclose(h);
}

osp_sym_t osp_dlsym(osp_dll_t h, const char* addr)
{
  void *p = dlsym(h, addr);
  return (osp_sym_t)p;
}
