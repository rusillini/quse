/* -*- C++ -*- */

#ifndef __thread_queue_h__
#define __thread_queue_h__

#include <config.h>
#include <mutex.h>
#include <event.h>

#define CMLIB_DISABLE_IOSTREAM
#include <except.h>
#include <vprocbase.h>

namespace cmlib {
  
  typedef v_proc_base InQueue;
  
  class Queue
  {
  public:
    struct in_queue_t
    {
      InQueue* node;
      in_queue_t* next;
    };
    
  public:
    Queue();
    virtual ~Queue();
    
  private:
    osp_event_t event_;
    osp_mutex_t mtx_;
    intp_ot size_;
    in_queue_t* queue_;
    in_queue_t* head_;
    in_queue_t* trail_;
    
  public:
    virtual void push(InQueue* node) throw(std::exception);
    virtual InQueue* pop() throw(std::exception);
    
    intp_ot size()
    {
      intp_ot tmp;
      osp_mutex_lock(&mtx_);
      tmp = size_;
      osp_mutex_unlock(&mtx_);
      return tmp;
    }
    
    bool is_empty() { return size()<1; }
    
    void clear(bool del=false);
  };
  
} // namespace cmlib

#endif // __cmlib_thread_queue_h__
