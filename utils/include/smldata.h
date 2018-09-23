#ifndef __cmlib_smldata_h__
#define __cmlib_smldata_h__

#include <config.h>
#include <memory.h>

#define CMLIB_DISABLE_IOSTREAM  
#include <cmtypes.h>
#include <except.h>
#include <bufstream.h>
#include <encoding.h>

namespace cmlib {
  
  class SmlNode;
  
  bool SMLinitEncodingHandler(const char* tablePath=NULL);
 
  cmlib::EncodingHandler& SMLencodingHandler();
 
  void transcodeSML(SmlNode& root, cmlib::EncodingHandler& h,
                    const char* encFrom, const char* encTo=OSP_ENCODING);
  
  inline void transcodeSML(SmlNode& root, const char* encFrom,
                           const char* encTo=OSP_ENCODING)
  {
    transcodeSML(root, SMLencodingHandler(), encFrom, encTo);
  }
  void doTranscodeSML(SmlNode& curr, EncodingHandler& h,
                      const EncodingHandler::tConvData* dtFrom,
                      const EncodingHandler::tConvData* dtTo);
  
  void XMLwriteFile(FILE* f, const SmlNode& root,
                    const char* enc = "", bool bInsertIndent=false)
    throw(std::exception);
  void XMLwriteFile(const char* fname, const SmlNode& root,
                    const char* enc = "", bool bInsertIndent=false)
    throw(std::exception);
  void XMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len, const SmlNode& root,
                      const char* enc = "", osp_malloc_t dllm_malloc=osp_malloc,
                      bool bInsertIndent=false)
    throw(std::exception);
  void XMLwriteStream(basestream& s, const SmlNode& root,
                      const char* enc = "", bool bInsertIndent=false)
    throw(std::exception);
  
  //------------------------------------------------------------
 

#ifndef SML_XML2SML_PATH  
# define SML_XML2SML_PATH "$SRVTHOME/bin/xml2sml" OSP_EXE_EXT
#endif
  
  void XMLreadFileExt(SmlNode& root,
      const char* fPath,
      const char* cachePath=NULL,
      unsigned long msecWait = 2000,
      const char* UtilPath=SML_XML2SML_PATH)
    throw(std::exception);
  
  void XMLreadBufferExt(SmlNode& root,
      osp_void_ptr buf, u_intp_ot len,
      unsigned long msecWait = 2000,
      const char* UtilPath=SML_XML2SML_PATH)
    throw(std::exception);
    
  //------------------------------------------------------------
  
  void SMLwriteFile(FILE* f, const SmlNode& root, const char* enc=NULL)
    throw(std::exception);
  void SMLwriteFile(const char* fname, const SmlNode& root, const char* enc=NULL)
    throw(std::exception);
  void SMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len,
                      const SmlNode& root, osp_malloc_t dllm_malloc=osp_malloc,
                      const char* enc=NULL)
    throw(std::exception);
  void SMLwriteStream(basestream& s, const SmlNode& root, const char* enc=NULL)
    throw(std::exception);
  
  //------------------------------------------------------------
  
  void SMLreadFile(FILE* f, SmlNode& root, std::string* enc=NULL, bool clean = true)
    throw(std::exception);
  void SMLreadFile(const char* fname, SmlNode& root, std::string* enc=NULL, bool clean = true)
    throw(std::exception);
  void SMLreadBuffer(osp_void_ptr buf, u_intp_ot len,
                     SmlNode& root, std::string* enc=NULL, bool clean = true)
    throw(std::exception);
  
  
  //------------------------------------------------------------
  
  class SmlNode
  {
    
    friend void XMLwriteFile(FILE* f, const SmlNode& root,
                             const char* enc, bool bInsertIndent)
      throw(std::exception);
    friend void XMLwriteFile(const char* fname, const SmlNode& root,
                             const char* enc, bool bInsertIndent)
      throw(std::exception);
    friend void XMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len, const SmlNode& root,
                               const char* enc, osp_malloc_t dllm_malloc,
                               bool bInsertIndent)
      throw(std::exception);
    friend void XMLwriteStream(basestream& s, const SmlNode& root,
                               const char* enc, bool bInsertIndent)
      throw(std::exception);
    friend void SMLwriteFile(FILE* f, const SmlNode& root, const char* enc)
      throw(std::exception);
    friend void SMLwriteFile(const char* fname, const SmlNode& root, const char* enc)
      throw(std::exception);
    friend void SMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len,
                               const SmlNode& root, osp_malloc_t dllm_malloc,
                               const char* enc)
      throw(std::exception);
    friend void SMLwriteStream(basestream& s, const SmlNode& root, const char* enc)
      throw(std::exception);
    friend void transcodeSML(SmlNode& root, cmlib::EncodingHandler& h,
                             const char* encFrom, const char* encTo);
    friend void doTranscodeSML(SmlNode& curr, EncodingHandler& h,
                               const EncodingHandler::tConvData* dtFrom,
                               const EncodingHandler::tConvData* dtTo);
    
  public:
    typedef std::vector<SmlNode*> tChildren;
    typedef std::map<std::string, std::string> tAttributes;
    typedef std::pair<std::string, std::string> tAttrValue;
    
  protected:
    
    SmlNode& operator=(const SmlNode&) { return *this; }
    SmlNode(const SmlNode&) {}
    
  protected:
    tAttributes attributes_;
    tChildren children_;
    std::string name_;
    std::string text_;
    SmlNode* parent_;
    unsigned int npos_line_;
    unsigned int npos_column_;
  public:
    SmlNode(const char* name=NULL, SmlNode* parent=NULL);
    virtual ~SmlNode();
    virtual void clear();
    virtual void clear_child();
    
    virtual void name(const char name[]) { name_ = name; }
    virtual const char* name() const { return name_.c_str(); }
    std::string& nameRef() { return name_; }
    
    SmlNode* parent() const { return parent_; }
    void parent(SmlNode* p) { parent_ = p; }
    
    virtual void text(const char text[]) { text_ = text; }
    virtual const char* text() const { return text_.c_str(); }
    std::string& textRef() { return text_; }
    
    unsigned int npos_line() const { return npos_line_; }
    void npos_line(unsigned int v) { npos_line_ = v; }
    unsigned int npos_column() const { return npos_column_; }
    void npos_column(unsigned int v) { npos_column_ = v; }
    
    unsigned int attrCount() const { return (unsigned int)attributes_.size(); }
    unsigned int nodeCount() const { return (unsigned int)children_.size(); }
    
    void setAttr(const tAttrValue& val)
    { setAttr(val.first.c_str(), val.second.c_str()); }
    void setAttr(const char* name, const char* value)
    {
      if ( name==NULL ) name = "";
      if ( value==NULL ) value = "";
      attributes_[name] = value;
    }
    void setAttr(const char* name, bool value)
    { setAttr(name, value ? "1" : "0"); }
    void setAttr(const char* name, long value)
    {
      char buf[16]; sprintf(buf, "%ld", value);
      setAttr(name, buf);
    }
    void setAttr(const char* name, unsigned long value)
    {
      char buf[16]; sprintf(buf, "%lu", value);
      setAttr(name, buf);
    }
    
    tAttributes readAllAttr() const
    {
      return attributes_;
    }
    
    bool hasAttr(const char* name) const
    {
      return attributes_.find(name) != attributes_.end();
    }
    const char* getAttr(const char* name) const
    {
      tAttributes::const_iterator it = attributes_.find(name);
      if ( it == attributes_.end() )
        return "";
      return (*it).second.c_str();
    }
    long getLongAttr(const char* name) const
    {
      const char* tmp = getAttr(name);
      return atol(tmp);
    }
    bool getBoolAttr(const char* name) const
    {
      const char* tmp = getAttr(name);
      return (strcmp(tmp, "1")==0);
    }
    
    
    bool eraseAttr(const char* name)
    {
      tAttributes::iterator it = attributes_.find(name);
      if ( it == attributes_.end() )
        return false;
      attributes_.erase(it);
      return true;
    }
    
   
    bool eqName(const char* name) const
    {
      const char* p = name_.c_str();
      if ( name==p )
        return true;
      if ( name==NULL || p==NULL )
        return false;
      while ( *name && *p )
      {
        if ( *name!=*p )
          return false;
        ++name; ++p;
      }
      return (*name==*p);
    }
    
    SmlNode& node(const char* name)
      { return *addChildNode(name); }
    
    SmlNode* addChildNode(const char* name = "");
    
    void addChildNode(SmlNode* child)
      { children_.push_back(child); }
    
    SmlNode* getChildNode(unsigned int i);
    const SmlNode* getChildNode(unsigned int i) const;
    
    SmlNode* getChildNodeByAtrValue(const char* atr_name,
                                    const char* atr_value);
    
    SmlNode* getChildNodeByName(const char* name);
    
   
    virtual bool eraseChild(SmlNode* child)
    {
      tChildren::iterator it;
      for (it = children_.begin(); it!=children_.end(); it++)
        {
          if ( (*it)==child )
            {
              delete child;
              children_.erase(it);
              return true;
            }
        }
      return false;
    }
    
   
    virtual bool ripOffChild(SmlNode* child)
    {
      tChildren::iterator it;
      for (it = children_.begin(); it!=children_.end(); it++)
        {
          if ( (*it)==child )
            {			  
              children_.erase(it);
              return true;
            }
        }
      return false;
    }
    
   
    void eraseSelf()
    {
      if ( parent_==NULL )
        delete this;
      else if ( parent_->eraseChild(this)==false )
        delete this;
    }
    
  protected:
    virtual SmlNode* newNode(const char* name, SmlNode* p) const
      { return new SmlNode(name, p); }
    
    virtual void deleteNode(SmlNode* node) const
      { delete node; }
    
  }; // class SmlNode
  
  
} // namespace cmlib


#endif // __cmlib_smldata_h__
