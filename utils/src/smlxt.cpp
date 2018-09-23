#include <config.h>
#define DISABLE_IOSTREAM
#include <cmtypes.h>
#include <smldata.h>
#include <envparm.h>
#include <encoding.h>
#include <smlxt.h>
#include <expat.h>


struct xmlDataDesc
{
  cmlib::SmlNode* curr;
  cmlib::SmlNode* root;
  unsigned long len;
  XML_Parser parser;
  bool bTextContextLoss;

  xmlDataDesc()
  {
    curr = NULL;
    root = NULL;
    len = 0;
    parser = NULL;
  }
};


inline bool
isNiceChar(char c)
{
  switch (c)
    {
    case ' ': case '\t':
    case '\n': case '\r':
      return false;
    default:
      return true;
    };
}

inline void
optStringSpace(std::string& s)
{
  std::string::iterator it;

  for ( it=s.end(); it!=s.begin(); it--)
    {
      if ( isNiceChar(*(it-1)) )
        break;
    }
  if ( it!=s.end() )
    s.erase(it, s.end());

  for ( it=s.begin(); it!=s.end(); it++)
    {
      if ( isNiceChar(*it) )
        break;
    }
  if ( it!=s.begin() )
    s.erase(s.begin(), it);
}


static void
startElement(void *userData, const char *name, const char **atts)
{
  // ������ ����: ���������� �� ����� ���
  xmlDataDesc* p = (xmlDataDesc*)userData;
  if ( p==NULL ) return;

  cmlib::SmlNode* node = p->curr;
  if ( node )
    node = node->addChildNode(name);
  else
    {
      node = p->root;
      node->nameRef() = name;
    }

  for (int i = 0; atts[i]; i += 2)
    node->setAttr(atts[i], atts[i+1]);

  p->curr = node;
  p->len += 1;

  node->npos_line((unsigned int)XML_GetCurrentLineNumber(p->parser));
  node->npos_column((unsigned int)XML_GetCurrentColumnNumber(p->parser));
}

static void
endElement(void *userData, const char*)
{
  // ��������� ���: ������������ �������
  xmlDataDesc* p = (xmlDataDesc*)userData;
  if ( p==NULL ) return;

  if ( p->len > 0 )
    p->len -= 1;

  if ( p->curr )
    {
      optStringSpace(p->curr->textRef());
      p->curr = p->curr->parent();
    }
}

static void
startCData(void *userData)
{
  // ������ �������� !CDATA: ���������� �� ����� ���
  xmlDataDesc* p = (xmlDataDesc*)userData;
  if ( p==NULL ) return;

  cmlib::SmlNode* node = p->curr;
  if ( node )
    node = node->addChildNode("!CDATA");
  else
    return;

  p->curr = node;
  p->len += 1;
  node->npos_line((unsigned int)XML_GetCurrentLineNumber(p->parser));
  node->npos_column((unsigned int)XML_GetCurrentColumnNumber(p->parser));
}

static void
endCData(void *userData)
{
  endElement(userData, "!CDATA");
}

static void
handleCData(void *userData, const XML_Char *s, int len)
{
  // ��������� ������ ����������� � ��� ��������
  // � �������� text �������� ����.
  xmlDataDesc* p = (xmlDataDesc*)userData;
  if ( p->curr )
    p->curr->textRef().append((const char*)s, len);
}


static int
unknownEncoding(void *handlerData,
                const XML_Char *name, XML_Encoding *info)
{
  if ( name==NULL || info==NULL || handlerData==NULL )
    return 0;

  cmlib::EncodingHandler *h = (cmlib::EncodingHandler*)handlerData;
  if ( h->fillWideArray(info->map, name)==false )
    return 0;

  info->data = NULL;
  info->convert = NULL;
  info->release = NULL;

  return 1;
}


//----------------------------------------------------------------------

void cmlib::
XMLreadFile(FILE* f, cmlib::SmlNode& root, bool clean)
  throw(std::exception)
{
  static const char* szThisMethod = "XMLreadFile";
  if (clean)
    root.clear();
  if ( f==NULL )
    cmlib_throw(szThisMethod, "Invalid arguments");

  xmlDataDesc data;
  data.curr = NULL;
  data.root = &root;

  XML_Parser p = XML_ParserCreate(NULL);
  if ( ! p )
    cmlib_throw(szThisMethod, "Unable to create XML parser");

  data.parser = p;
  XML_SetElementHandler(p, startElement, endElement);
  XML_SetUserData(p, &data);

  cmlib::EncodingHandler& h = cmlib::SMLencodingHandler();
  if ( h.hasCodeTables()==false )
    {
      fseek(stderr, 0, SEEK_END);
      fprintf(stderr, "(cmlib) Warning: transcoder not initialized at %s\n", szThisMethod);
      cmlib::SMLinitEncodingHandler();
    }
  XML_SetUnknownEncodingHandler(p, unknownEncoding, (void*)&h);
  XML_SetCharacterDataHandler(p, handleCData);
  XML_SetCdataSectionHandler(p, startCData, endCData);

  char xbuf[512] = "";
  size_t cnt;
  const int bufSize = 16384;

  while ( ! feof(f) )
    {
      void *buf = XML_GetBuffer(p, bufSize);
      if ( buf==NULL )
        {
          XML_ParserFree(p);
          cmlib_throw(szThisMethod, "Memory allocation failed");
        }

      cnt = fread(buf, 1, bufSize, f);
      if ( ferror(f) )
        {
          std::string szErrMsg = std::string("File read error: ") + strerror(errno);
          XML_ParserFree(p);
          cmlib_throw(szThisMethod, szErrMsg.c_str());
        }
      if ( ! XML_ParseBuffer(p, cnt, cnt==0) )
        {
          sprintf(xbuf, "Parse error at line %d: %s",
                  (int) XML_GetCurrentLineNumber(p),
                  XML_ErrorString(XML_GetErrorCode(p)));
          XML_ParserFree(p);
          cmlib_throw(szThisMethod, xbuf);
        }
    }

  XML_ParserFree(p);
}

void cmlib::
XMLreadFile(const char* fname, cmlib::SmlNode& root, bool clean)
  throw(std::exception)
{
  static const char* szThisMethod = "XMLreadFile";
  if (clean)
    root.clear();
  if ( fname==NULL )
    cmlib_throw(szThisMethod, "Invalid arguments");

  FILE* f = fopen(fname, "rb");
  if ( f==NULL )
    {
      std::string szErrMsg = std::string("Unable to open file `") +
        std::string(fname) + std::string("\': ") + strerror(errno);
      cmlib_throw(szThisMethod, szErrMsg);
    }

  try {
    cmlib::XMLreadFile(f, root, clean);
  } catch(const std::exception&) {
    fclose(f);
    throw;
  }
  fclose(f);
}

void cmlib::
XMLreadBuffer(osp_void_ptr buf,
              size_t len, cmlib::SmlNode& root, bool clean)
  throw(std::exception)
{
  static const char* szThisMethod = "XMLreadBuffer";
  if (clean)
    root.clear();
  if ( buf==NULL || len==0 )
    return; // Nothing to do

  xmlDataDesc data;
  data.curr = NULL;
  data.root = &root;

  XML_Parser p = XML_ParserCreate(NULL);
  if ( ! p )
    cmlib_throw(szThisMethod, "Unable to create XML parser");

  data.parser = p;
  XML_SetElementHandler(p, startElement, endElement);
  XML_SetUserData(p, &data);

  cmlib::EncodingHandler& h = cmlib::SMLencodingHandler();
  if ( h.hasCodeTables()==false )
    {
      fseek(stderr, 0, SEEK_END);
      fprintf(stderr, "(cmlib) Warning: transcoder not initialized at %s\n", szThisMethod);
      cmlib::SMLinitEncodingHandler();
    }
  XML_SetUnknownEncodingHandler(p, unknownEncoding, (void*)&h);

  if ( ! XML_Parse(p, (const char*)buf, len, 1) )
    {
      char xbuf[512];
      sprintf(xbuf, "Parse error at line %d: %s",
              (int) XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      XML_ParserFree(p);
      cmlib_throw(szThisMethod, xbuf);
    }

  XML_ParserFree(p);
}
