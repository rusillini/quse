/* -*- C++ -*- */

#ifndef __dlutils_h__
#define __dlutils_h__

#include <config.h>
#include <dll.h>
#define CMLIB_DISABLE_IOSTREAM
#include <cmtypes.h>

template<class F> inline bool
getDllSymbol(osp_dll_t dll, const char* name, F& ptr)
{
  ptr = (F)osp_dlsym(dll, name);
  return (ptr!=NULL);
}

inline void path_replace_sym(std::string& s)
{
  std::string::iterator it;
  for (it = s.begin(); it!=s.end(); it++)
    {
      switch ( *it ) {
      case '/':
      case '\\':
      case ':':
        *it = '_';
      }
    }
}

#endif // __dlutils_h__
