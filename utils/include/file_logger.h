/* -*- C++ -*- */

#ifndef __file_logger_h__
#define __file_logger_h__

#include <config.h>

#ifndef USE_UNLOCKED_LOGGER
# include <mutex.h>
#endif // USE_UNLOCKED_LOGGER

#define CMLIB_DISABLE_IOSTREAM
#include <cmtypes.h>
#include <auto_create.h>
#include <envparm.h>

namespace cmlib {

class FileLogger
{
public:
  typedef cmlib::auto_release_ptr<FileLogger> ptr;

protected:
  typedef struct tag_guard_t {
#ifdef USE_UNLOCKED_LOGGER
    tag_guard_t(osp_mutex_t&) {}
    ~tag_guard_t() {}
#else
    tag_guard_t(osp_mutex_t& mtx) {mtx_ = mtx; osp_mutex_lock(&mtx_);}
    ~tag_guard_t() {osp_mutex_unlock(&mtx_);}
  private:
    osp_mutex_t mtx_;
#endif // USE_UNLOCKED_LOGGER
  } guard_t;
  
  FILE *fd_;
  bool bOwnHandle_;
  bool bLeadingSymbols_;
  osp_mutex_t mtx_;
  
public:
  FileLogger(const char* fname)
  {
#ifndef USE_UNLOCKED_LOGGER
    osp_mutex_init(&mtx_);
#endif //USE_UNLOCKED_LOGGER
    fd_ = NULL;
    bOwnHandle_ = true;
    if ( fname==NULL ) fname="";
    fd_ = fopen(makeFullString(fname).c_str(), "at");
    if (fd_==NULL)
      {
        bOwnHandle_ = false;
        fd_ = stderr;
        fseek(fd_, 0, SEEK_END);
        fprintf(fd_, "Warning: Logger failed to open \'%s\'. "
                "Reassigns to stderr.\n", fname ? fname : "<null>");
      }
    
    bLeadingSymbols_ = false;
    ::tzset();
  }
  
  FileLogger(FILE *fd, bool own=false)
  {
#ifndef USE_UNLOCKED_LOGGER
    osp_mutex_init(&mtx_);
#endif //USE_UNLOCKED_LOGGER
    if (fd==NULL)
      {
        fd_ = stderr;
        bOwnHandle_ = false;
        fseek(fd_, 0, SEEK_END);
        fprintf(fd_, "Warning: Logger got NULL as file descriptor. "
                "Reassigns to stderr.\n");
      }
    else
      {
        fd_ = fd;
        bOwnHandle_ = own;
      }
    
    bLeadingSymbols_ = false;
    tzset();
  }
  
  ~FileLogger()
  {
    osp_mutex_destroy(&mtx_);
    if (bOwnHandle_)
      fclose(fd_);
  }
  
  void LeadingSymbols(bool b)
  {
    guard_t guard(mtx_);
    bLeadingSymbols_ = b;
  }
  
  bool LeadingSymbols()
  {
    guard_t guard(mtx_);
    return bLeadingSymbols_;
  }
  
  void write_line()
  {
    guard_t guard(mtx_);
    fseek(fd_, 0, SEEK_END);
    for (int i=0; i<80; ++i)
      fputc('-', fd_);
    fputc('\n', fd_);
    fflush(fd_);
  }

  int printf(const char *format, ...);
  int printf_now(const char *format, ...);
  int vprintf(const char *format, va_list v);
  int vprintf_now(const char *format, va_list v);
};

} // namespace cmlib

inline int cmlib::FileLogger::
vprintf(const char *format, va_list v)
{
  guard_t guard(mtx_);
  fseek(fd_, 0, SEEK_END);
  register int n = vfprintf(fd_, format, v);
  fflush(fd_);
  return n;
}


inline int cmlib::FileLogger::
printf(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
  int n = this->vprintf(format, vl);
  va_end(vl);
  return n;
}

inline int cmlib::FileLogger::
vprintf_now(const char *format, va_list v)
{
  guard_t guard(mtx_);
  fseek(fd_, 0, SEEK_END);
  
  if (bLeadingSymbols_)
  {
    fprintf(fd_, ">>> ");
  }
  
  time_t t = time(&t);
  char *ctm = ctime(&t); ctm[24] = '\0';
  fprintf(fd_, "%s:  ", ctm);
  
  int n = vfprintf(fd_, format, v);
  
  fprintf(fd_, "\n");
  fflush(fd_);
  
  return n;
}

inline int cmlib::FileLogger::
printf_now(const char *format, ...)
{
  va_list vl;
  va_start(vl, format);
  int n = this->vprintf_now(format, vl);
  va_end(vl);
  
  return n;
}

#endif // __file_logger_h__
