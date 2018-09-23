// -*- C++ -*-

#ifndef __cmlib_smlproc_h__
#define __cmlib_smlproc_h__

#include "config.h"
#define CMLIB_DISABLE_IOSTREAM  
#include "cmtypes.h"
#include "vprocbase.h"
#include "smldata.h"

namespace cmlib {
  
  class SmlHandlerBase
  {
  public:
    SmlHandlerBase();
    virtual ~SmlHandlerBase();
    
  public:
    virtual SmlHandlerBase* open(SmlNode* n, v_proc_base* data)
      throw(std::exception) = 0;
    virtual void proc_before(v_proc_base* data)
      throw(std::exception) = 0;
    virtual void proc_after(v_proc_base* data)
      throw(std::exception) = 0;
  };
  
  
  class SmlHandler : public SmlHandlerBase
  {
  private:
    SmlHandlerBase* parent_;
    SmlNode* node_;
    
  public:
    SmlHandler(SmlHandlerBase* p, SmlNode* n);
    virtual ~SmlHandler();
    
    SmlNode* node() const { return node_; }
    SmlHandlerBase* parent() const { return parent_; }
    
  public:
    virtual SmlHandlerBase* open(SmlNode* n, v_proc_base* data)
      throw(std::exception);
    virtual void proc_before(v_proc_base* data)
      throw(std::exception);
    virtual void proc_after(v_proc_base* data)
      throw(std::exception);
  };
  
  void SmlProcessTree(SmlHandlerBase* handler, SmlNode* root,
                      v_proc_base* data = NULL)
    throw(std::exception);
  
} // namespace cmlib

#endif // __cmlib_smlproc_h__
