// -*- C++ -*-
#include <config.h>
#include <memory.h>
#define CMLIB_DISABLE_IOSTREAM
#include <except.h>
#include <envparm.h>
#include <strutils.h>

// _envparm
cmlib::_envparm::
_envparm(const char* name)
{
  if ( name==NULL ) name = "Globals";
  int err = osp_initenv(name);
  if ( err != 0 )
    {
      fseek(stderr, 0, SEEK_END);
      fprintf(stderr, "(cmlib) osp_initenv(\"%s\") failed (%d). "
              "Program malfunction may result.\n",
              name, err);
    }
}

cmlib::_envparm::~_envparm()
{}

//----------------------------------------------------------
// strutils
int cmlib::string_ref::
compare(const char* s) const
{
  register const char* t;

  if ( s_==s ) return 0;
  if ( s_==NULL )
    return (*s ? -1 : 0);
  if ( s==NULL )
    return (*s_ ? 1 : 0);
  t = s_;

  while ( *s && *t )
    {
      if ( *t < *s )
        return -1;
      if ( *t > *s )
        return 1;
      ++t, ++s;
    }
  if ( *t < *s ) return -1;
  return (*t == *s) ? 0 : 1;
}
