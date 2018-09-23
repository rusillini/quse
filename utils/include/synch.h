#ifndef __cmlib_synch_h__
#define __cmlib_synch_h__

#include <mutex.h>
#include <rwsync.h>

namespace cmlib {
  
  class mutex
  {
  protected:
    osp_mutex_t mtx_;
 
  public:
    mutex()
    {
      osp_mutex_init(&mtx_);
    }
    ~mutex()
    {
      osp_mutex_destroy(&mtx_);
    }
    bool lock()
    {
      return (osp_mutex_lock(&mtx_)==0);
    }
    bool try_lock()
    {
      return (osp_mutex_trylock(&mtx_)==0);
    }
    bool unlock()
    {
      return (osp_mutex_unlock(&mtx_)==0);
    }
  };
  
  template<class M>
  class locker
  {
  protected:
    M& m_;
    int c_;

  public:
    locker(M& m, bool lockNow=true)
      : m_(m), c_(0)
    {
      if ( lockNow )
        {
          c_++;
          m_.lock();
        }
    }
    
    ~locker()
    {
      if ( c_ > 0 )
        m_.unlock();
      c_ = 0;
    }
    
    bool lock()
    {
      if ( c_ > 0 )
        {
          c_ ++;
          return true;
        }
      if ( m_.lock() )
        {
          c_++;
          return true;
        }
      return false;
    }
    
    bool unlock()
    {
      if ( c_<1 )
        return false;
      if ( --c_ < 1 )
        return m_.unlock();
      return true;
    }
  };
  
  
  class locker_ex
  {
  private:
    osp_mutex_t *mtx_;
    
  public:
    locker_ex(osp_mutex_t *mtx)
      : mtx_(mtx)
    {
      osp_mutex_lock(mtx_);
    }
    
    ~locker_ex()
    {
      osp_mutex_unlock(mtx_);
    }
  };
  
  class lock_read
  {
  private:
    osp_rwsync_t* sync_;

  public:
    lock_read(osp_rwsync_t* sync)
      : sync_(sync)
    {
      if ( sync_ )
        osp_rwsync_lock_r(sync_, -1);
    }
    
    ~lock_read()
    {
      if ( sync_ )
        osp_rwsync_unlock_r(sync_);
    }
  };
  
  class lock_write
  {
  private:
    osp_rwsync_t* sync_;
    
  public:
    lock_write(osp_rwsync_t* sync)
      : sync_(sync)
    {
      if ( sync_ )
        osp_rwsync_lock_w(sync_, -1);
    }
    
    ~lock_write()
    {
      if ( sync_ )
        osp_rwsync_unlock_w(sync_);
    }
  };
} // namespace cmlib

#endif // __cmlib_synch_h__
