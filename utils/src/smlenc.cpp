#include <config.h>
#define DISABLE_IOSTREAM
#include <cmtypes.h>
#include <envparm.h>
#include <encoding.h>
#include <smldata.h>
#include <smlxt.h>


static class defaultEncodingHandler_t
{
protected:
  cmlib::EncodingHandler *h_;
  
public:
  defaultEncodingHandler_t()
  {
    h_ = NULL;
  }
  ~defaultEncodingHandler_t()
  {
    delete h_;
    h_ = NULL;
  }
  
  cmlib::EncodingHandler& handler()
  {
    if ( h_==NULL )
      h_ = new cmlib::EncodingHandler;
    return *h_;
  }
  
  bool addData(const char* path)
  {
    cmlib::EncodingHandler &h = handler();
    if ( path==NULL )
      path = "%SRVTHOME%/etc/codepages.dat";
    std::string aPath = cmlib::makeFullString(path);
    return h.addCodeTables(aPath.c_str());
  }
  
  bool fillWideArray(int map[256], const char* name)
  {
    cmlib::EncodingHandler &h = handler();
    return h.fillWideArray(map, name);
  }
} encodingHandler;


// Operations on internal encoding handler.
// Not thread safe, so be careful.
bool cmlib::
SMLinitEncodingHandler(const char* tablePath/*=NULL*/)
{
  return encodingHandler.addData(tablePath);
}

cmlib::EncodingHandler& cmlib::
SMLencodingHandler()
{
  return encodingHandler.handler();
}


struct ParseNodeDescr
{
  ParseNodeDescr(cmlib::SmlNode* node, size_t pos=0) :
    node_(node), pos_(pos) {}
  
  cmlib::SmlNode* node_;
  size_t pos_;
};


void cmlib::
doTranscodeSML(cmlib::SmlNode& curr, cmlib::EncodingHandler& h,
               const cmlib::EncodingHandler::tConvData* dtFrom,
               const cmlib::EncodingHandler::tConvData* dtTo)
{
  // Temporary string for conversion
  std::string tmpStr, tmpVal;
  // Iterators
  cmlib::SmlNode::tChildren::iterator itc;
  cmlib::SmlNode::tAttributes::iterator it;
  cmlib::SmlNode::tAttributes attrTmp;
  
  // Transcode tag name
  h.plainToPlain(curr.name(), tmpStr, dtFrom, dtTo);
  curr.name(tmpStr.c_str()); tmpStr = "";
  
  // Transcode tag text
  h.plainToPlain(curr.text(), tmpStr, dtFrom, dtTo);
  curr.text(tmpStr.c_str()); tmpStr = "";
  
  // Transcode tag attributes
  attrTmp.clear();
  for ( it=curr.attributes_.begin(); it!=curr.attributes_.end(); it++ )
    {
      tmpStr = ""; tmpVal = "";
      h.plainToPlain((*it).first.c_str(), tmpStr, dtFrom, dtTo);
      h.plainToPlain((*it).second.c_str(), tmpVal, dtFrom, dtTo);
      attrTmp[tmpStr] = tmpVal;
    }
  curr.attributes_ = attrTmp;
  
  // Transcode tag children
  for ( itc=curr.children_.begin(); itc!=curr.children_.end(); itc++ )
    {
      if ( (*itc)==NULL )
        continue;
      doTranscodeSML(*(*itc), h, dtFrom, dtTo);
    }
}


// SML tree complete transcoding
void cmlib::
transcodeSML(SmlNode& root, cmlib::EncodingHandler& h,
             const char* encFrom, const char* encTo/*=SML_XMLENC*/)
{
  // Transcoding tables
  const cmlib::EncodingHandler::tConvData* dtFrom = NULL;
  const cmlib::EncodingHandler::tConvData* dtTo = NULL;
  if ( strcmp(encFrom, "utf-8")!=0 )
    dtFrom = h.getCodingMagic(encFrom);
  if ( strcmp(encTo, "utf-8")!=0 )
    dtTo = h.getCodingMagic(encTo);
  
  doTranscodeSML(root, h, dtFrom, dtTo);
}
