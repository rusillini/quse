#ifndef __cmlib_envparm_h__
#define __cmlib_envparm_h__

#include "config.h"
#include "environ.h"
#include "memory.h"
#define CMLIB_DISABLE_IOSTREAM  
#include "cmtypes.h"


namespace cmlib
{
  
  class _envparm
  {
  public:
    _envparm(const char* name="Globals");
    virtual ~_envparm();
  };
  
  inline void normalizePath(std::string& path)
  {
    char* tmp = osp_str_norm(path.c_str());
    path = tmp;
    osp_free(tmp);
  }
  
  inline std::string normalizePath(const char* path)
  {
    char* tmp = osp_str_norm(path);
    std::string retVal(tmp);
    osp_free(tmp);
    return retVal;
  }
  
  inline std::string
  makeFullString(const char *szStr, bool bNormalize = true)
  {
    char* tmp = NULL;
    if ( bNormalize )
      tmp = osp_path_norm(szStr);
    else
      tmp = osp_str_norm(szStr);
    std::string ret(tmp);
    osp_free(tmp);
    return ret;
  }
  
  inline void expandPath(std::string& path)
  {
    char* tmp = osp_path_norm(path.c_str());
    path = tmp ? tmp : "";
    osp_free(tmp);
  }
  
} // namespace cmlib


#endif // __cmlib_envparm_h__
