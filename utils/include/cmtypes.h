#ifndef __cmlib_cmtypes_h__
#define __cmlib_cmtypes_h__

#include "config.h"

#include <typeinfo>
#include <exception>
#include <stdexcept>
#include <new>

#include <map>
#include <set>

#include <stack>
#include <list>
#include <queue>
#include <vector>
#include <string>

#ifndef CMLIB_DISABLE_IOSTREAM
# ifdef CC_OLD_IOSTREAM
#   include <iostream.h>
#   include <strstream.h>
#   include <fstream.h>
# else
#  include <iostream>
#  ifdef CC_HAS_SSTREAM
#    include <sstream>
#  else
#    include <strstream>
#  endif
#  include <fstream>
# endif
#endif


#ifdef CC_NO_STD_NS
#undef std
#define std
#endif

#ifdef CC_NO_WCHAR_T
typedef u_int16_ot wchar_t;
#endif

#define STL_COPY_IMPL(name,parent_type)         \
class name : public parent_type                 \
{                                               \
public:                                         \
  typedef parent_type parent;                   \
public:                                         \
  name() {}                                     \
  name(const parent& x) : parent(x) {}          \
  name& operator=(const name& x)                \
  {                                             \
    parent::operator=(x);                       \
    return *this;                               \
  }                                             \
}

#endif // __cmlib_cmtypes_h__
