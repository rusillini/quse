// -*- C++ -*-

inline void cmlib::ThreadBase::create()
  throw(std::exception)
{
  if ( osp_thread_create(&handle_,
                         thr_func, (void*)this, 0, 0) == -1 )
    throw cmlib::err_t("ThreadBase::create()",
                       "osp_thread_create() failed");
}

inline void cmlib::ThreadBase::terminate()
{
  impl_terminate();
}

inline bool cmlib::ThreadBase::join(intp_ot tm)
{
  if ( osp_event_wait(event_, tm, 1) == 0 )
    {
      osp_thread_join(&handle_, &retVal_);
      osp_event_reset(event_);
      return true;
    }
  return false;
}

inline void cmlib::ThreadPool::terminate()
{
  for (threads_it it = threads_.begin(); it != threads_.end(); it++) {
    if ( *it )
      (*it)->impl_terminate();
  }
}
