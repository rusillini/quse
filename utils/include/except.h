// -*- C++ -*-

// $Id: except.h,v 1.9 2009/08/13 12:40:38 akotov2 Exp $

#ifndef _cmlib_except_hpp_
#define _cmlib_except_hpp_

#include <string>
#include <exception>
#include <stdarg.h>
#include <stdio.h>

#define MAX_MSG_LEN 512

namespace cmlib {

  class Exception : public std::exception {
  public:
    Exception(const char *loc, const char *msg) :
      _loc(loc), _msg(msg)
    {
      update();
    }

    Exception(const char *msg) : _loc(""), _msg(msg)
    {
      update();
    }

    Exception(const char *loc, const char *msg, ...) {
      va_list args;
      _loc = loc;
      va_start(args, msg);
      create_msg(msg, args);
      va_end(args);
    }

    Exception(const char *msg, ...) {
      va_list args;
      _loc = "";
      va_start(args, msg);
      create_msg(msg, args);
      va_end(args);
    }

    Exception(std::string &loc, std::string &msg) :
      _loc(loc), _msg(msg)
    {
      update();
    }

    Exception(std::string &msg) : _msg(msg)
    {
      update();
    }

    Exception(const Exception& e) {
      _loc = e._loc;
      _msg = e._msg;
      update();
    }

    Exception& operator=(Exception& e) {
      _loc = e._loc;
      _msg = e._msg;
      update();
      return *this;
    }

    virtual ~Exception() throw() {}

    const char* get_loc() const { return _loc.c_str(); }
    const char* get_msg() const { return _msg.c_str(); }

    virtual const char *what() const throw() {
      return _desc.c_str();
    }
  protected:
    void update() {
      _desc = _loc.empty() ? _msg : "(" + _loc + ") " + _msg;
    }

    void create_msg(const char *msg, va_list args) {
      char buf[MAX_MSG_LEN];
      vsnprintf(buf, MAX_MSG_LEN, msg, args);
      _msg = buf;
    }
  protected:
    std::string _loc;
    std::string _msg;
    std::string _desc;
  };
}

#if defined(_DEBUG) || defined(_VERBOSE_DEBUG)
#define cmlib_throw(loc, msg)                                                         \
{                                                                                     \
char __line[7];                                                                       \
sprintf(__line, "%u", __LINE__);                                                      \
std::string __loc(std::string(__FILE__)+":"+__line+" - "+loc);                        \
std::string __msg(msg);                                                               \
throw cmlib::Exception(__loc, __msg);                                                 \
}
#else
#define cmlib_throw(loc, msg)                                                         \
{                                                                                     \
std::string __loc(loc);                                                               \
std::string __msg(msg);                                                               \
throw cmlib::Exception(__loc, __msg);                                                 \
}
#endif


#endif // _cmlib_except_hpp_
