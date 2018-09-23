/* -*- C++ -*- */

#ifndef __autocreate_h__
#define __autocreate_h__

#include <config.h>

namespace cmlib {
  
  template <typename T>
  class auto_release_ptr
  {
  public:
    auto_release_ptr(T* ptr = NULL)
    { instance_ = ptr; }
    
    ~auto_release_ptr()
    { release(); }
    
    void release()
    {
      if (instance_)
        delete instance_;
      instance_ = NULL;
    }
    
    auto_release_ptr& operator=(T* ptr)
    {
      if (instance_)
        delete instance_;
      instance_ = ptr;
      return *this;
    }
    
    auto_release_ptr& operator=(const auto_release_ptr& ptr)
    {
      if (instance_)
        delete instance_;
      instance_ = ptr.instance_;
      
      (const_cast<auto_release_ptr&>(ptr)).instance_ = NULL;
      return *this;
    }
    
    T* operator->() { return instance_; }
    T& operator*() { return *instance_; }
    const T& operator*() const { return *instance_; }
    
    T* in() const { return instance_; }
    
    T* retn()
    {
      T* p = instance_;
      instance_ = NULL;
      return p;
    }
    
    bool is_nil() const { return (instance_ == NULL); }
    
    bool operator==(const auto_release_ptr<T>& p)
    { return (instance_ == p.instance_); }
    bool operator!=(const auto_release_ptr<T>& p)
    { return (instance_ != p.instance_); }
    bool operator==(const T* p) { return (instance_ == p); }
    bool operator!=(const T* p) { return (instance_ != p); }
    
  private:
    T* instance_;
  };

  
  template <typename T>
  class auto_create_ptr
  {
  public:
    auto_create_ptr(T* p = NULL)
    { instance_ = p; }
    
    ~auto_create_ptr()
    { release(); }
    
    T* operator->() { return get(); }
    T& operator*() { return *get(); }
    
    void release()
    {
      if (instance_)
        delete instance_;
      instance_ = NULL;
    }
    
    T* in() const
    { return instance_; }
    
    T* retn()
    {
      T* p = instance_;
      instance_ = NULL;
      return p;
    }
    
    T* get()
    {
      if (!instance_)
        instance_ = new T;
      return instance_;
    }
    
    bool is_nil() const { return (instance_ == NULL); }
    
    bool operator==(const auto_create_ptr<T>& p)
    { return (instance_ == p.instance_); }
    bool operator!=(const auto_create_ptr<T>& p)
    { return (instance_ != p.instance_); }
    bool operator==(const T* p) { return (instance_ == p); }
    bool operator!=(const T* p) { return (instance_ != p); }
    
  private:
    T* instance_;
  };
  
  
  template <typename T>
  class auto_array
  {
    T* instance_;
    u_int32_ot len_;
    u_int32_ot size_;
  public:
    auto_array(u_int32_ot len=0, u_int32_ot size=0)
    {
      if (size<len)
        size = len;
      
      if (size>0)
        {
          instance_ = new T [size];
          assert(instance_!=0);
        }
      else
        instance_ = NULL;
      len_ = len;
      size_ = size;
    }
    
    ~auto_array()
    { release(); }
    
    void release()
    {
      if (instance_)
        delete[] instance_;
      instance_ = NULL;
      len_ = size_ = 0;
    }
    
    void copy(const T* b, u_int32_ot len)
    {
      alloc(len);
      if ( len ) memcpy(instance_, b, len);
    }
    
    void alloc(u_int32_ot len, u_int32_ot size=0)
    {
      if (size<len)
        size = len;
      
      if (size==0)
      {
        release();
        return;
      }
      
      if (size>size_)
        release();
      
      len_ = len;
      size_ = (size_>size) ? size_ : size;
      if (instance_==NULL)
      {
         instance_ = new T [size_];
         assert(instance_!=0);
      }
    }
   
    T& operator[] (u_int32_ot pos)
    {
      assert( (int(pos)>=0) && (pos<len_) );
      return instance_[pos];
    }
    
    const T& operator[] (u_int32_ot pos) const
    {
      assert( (int(pos)>=0) && (pos<len_) );
      return instance_[pos];
    }
    
    u_int32_ot size() const { return len_; }
    u_int32_ot total() const { return size_; }
    u_int32_ot type_size() const { return (u_int32_ot)sizeof(T); }
    
    T* buffer() { return instance_; }
    const T* buffer() const { return instance_; }
    
    T* operator()() { return instance_; }
    const T* operator()() const { return instance_; }
    
    T* detach()
    {
      T* buf = instance_;
      instance_ = NULL;
      len_ = 0;
      return buf;
    }
  };
    
  template <typename T>
  class zero_ptr
  {
  private:
    T*& ptr_;
    
  public:
    zero_ptr(T*& ptr, T* val=NULL)
      : ptr_(ptr)
    {
      ptr_ = val;
    }
    
    ~zero_ptr()
    {
      ptr_ = NULL;
    }
  };
  
} // namespace cmlib

#endif // __autocreate_h__
