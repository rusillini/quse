/* -*- C++ -*- */

#ifndef __cmlib_thread_pool_h__
#define __cmlib_thread_pool_h__

#include <config.h>
#define CMLIB_DISABLE_IOSTREAM
#include <cmtypes.h>
#include <except.h>

#include <thread.h>
#include <mutex.h>
#include <event.h>
#include <tmutil.h>

namespace cmlib {
  
  class ThreadPool;
  
  class ThreadBase
  {
    friend class ThreadPool;
  private:
    ThreadBase(const ThreadBase&) {}
    ThreadBase& operator=(const ThreadBase&)
    { return *this; }
    
  private:
    osp_thread_t handle_;
    intp_ot retVal_;
    bool isTerminated_;
    osp_event_t event_;
    bool self_event_;
    
  public:
    ThreadBase(osp_event_t e = OSP_EVENT_BAD);
    virtual ~ThreadBase();
    
    osp_thread_t handle() const { return handle_; }
    bool isTerminated() const { return isTerminated_; }
    bool getRetVal(intp_ot& ret) const;
    
  protected:
    
    virtual bool impl_prepare();
    virtual intp_ot impl_run();
    virtual void impl_done();
    virtual void impl_terminate();
    
  public:
    
    void create() throw(std::exception);
    void terminate();
    bool join(intp_ot tm);
    
  protected:
    static intp_ot thr_func(void* arg);
  };
  
  
  class ThreadPool
  {
  public:
    typedef std::list<ThreadBase*> threads_t;
    typedef threads_t::iterator threads_it;
    
  protected:
    virtual ThreadBase* new_thread() throw(std::exception) = 0;
    
  public:
    ThreadPool();
    virtual ~ThreadPool();
    
  private:
    threads_t threads_;
  protected:
    osp_event_t event_;
    
  public:
    void create(intp_ot n) throw(std::exception);
    void terminate();
    bool join(intp_ot tm = -1);
    
  private:
    void check_threads();
  };
  
  
  template<class T>
  class ThreadPoolT : public ThreadPool
  {
  protected:
    virtual ThreadBase* new_thread() throw(std::exception)
    {
      return new T(event_);
    }
    
  public:
    ThreadPoolT() {}
    virtual ~ThreadPoolT() {}
  };
  
} // namespace cmlib


#include "thread_pool.i"

#endif // __cmlib_thread_pool_h__
