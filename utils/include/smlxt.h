#ifndef __cmlib_smlxt_h__
#define __cmlib_smlxt_h__

#include <smldata.h>
#include <errno.h>

namespace cmlib {
  
  void XMLreadFile(FILE* f, SmlNode& root,
                   bool clean = true)
    throw(std::exception);
  void XMLreadFile(const char* fname, SmlNode& root,
                   bool clean = true)
    throw(std::exception);
  void XMLreadBuffer(osp_void_ptr buf, size_t len,
                     SmlNode& root, bool clean = true)
    throw(std::exception);
  
} // namespace cmlib


#endif // __cmlib_smlxt_h__
