#include <config.h>
#define CMLIB_DISABLE_IOSTREAM
#include <bufstream.h>
#include <encoding.h>
#include <smldata.h>

#include <stdio.h>
#include <assert.h>

#define SML_HEADER "SML_0.1"


//-----------------------------------------------------------------

cmlib::SmlNode::SmlNode(const char* name, SmlNode* parent)
{
  name_ = name ? name : "";
  parent_ = parent;
  npos_line_ = npos_column_ = 0;
}

cmlib::SmlNode::~SmlNode()
{
  clear();
}

void cmlib::SmlNode::clear()
{
  attributes_.clear();
  name_ = ""; text_ = "";
  parent_ = NULL;
  clear_child();
}

void cmlib::SmlNode::clear_child()
{
  for ( tChildren::iterator it = children_.begin();
        it != children_.end(); it++ )
    {
      deleteNode(*it);
    }
  children_.clear();
}

cmlib::SmlNode* cmlib::SmlNode::
getChildNode(unsigned int i)
{
  if (i >= children_.size())
    return NULL;
  return children_[i];
}

const cmlib::SmlNode* cmlib::SmlNode::
getChildNode(unsigned int i) const
{
  if (i >= children_.size())
    return NULL;
  return children_[i];
}

cmlib::SmlNode* cmlib::SmlNode::
getChildNodeByAtrValue(const char* atr_name,
                       const char* atr_value)
{
  for (u_intp_ot i = 0; i < children_.size(); i++)
    if ( strcmp(children_[i]->getAttr(atr_name), atr_value) == 0 )
      return children_[i];
  return NULL;
}

cmlib::SmlNode* cmlib::SmlNode::
getChildNodeByName(const char* name)
{
  for (u_intp_ot i = 0; i < children_.size(); i++)
    if ( strcmp(children_[i]->name(), name) == 0 )
      return children_[i];
  return NULL;
}

cmlib::SmlNode* cmlib::SmlNode::
addChildNode(const char* name)
{
  SmlNode* xml_data = newNode(name, this);
  assert(xml_data);
  children_.push_back(xml_data);
  return xml_data;
}


//-----------------------------------------------------------------
static std::string
normalizeXML(const char* text,
             bool bTrimLeft=false,
             bool bTrimRight=false)
{
  if ( text==NULL ) text = "";
  std::string retVal;

  if ( bTrimLeft )
  {
    while( (*text==' ') || (*text=='\t') )
      ++text;
  }

  unsigned long srcLen = strlen(text), i;
  if ( bTrimRight )
  {
    const char* xtext = text + (srcLen - 1);
    while ( (*xtext==' ') && (xtext>text) )
      --xtext;
    srcLen = xtext-text+1;
  }

  for(i=0; i<srcLen; ++text, ++i )
  {
    switch ( *text )
    {
    case '&':
      retVal += "&amp;"; break;
    case '<':
      retVal += "&lt;"; break;
    case '>':
      retVal += "&gt;"; break;
    case '\"':
      retVal += "&quot;"; break;
    default:
      retVal.insert(retVal.end(), *text);
    }
  }
  return retVal;
}

static inline void putcN(FILE* f, char c, u_intp_ot n)
{
  while (n--) fputc(c, f);
}
static inline void putcN(cmlib::basestream& b, char c, u_intp_ot n)
{
  while (n--) b.write(&c, 1);
}


struct ParseNodeDescr
{
  ParseNodeDescr(const cmlib::SmlNode* node, u_intp_ot pos=0) :
    node_(node), pos_(pos) {}

  const cmlib::SmlNode* node_;
  u_intp_ot pos_;
};


//-----------------------------------------------------------------
// Write data as clean XML.
void cmlib::
XMLwriteFile(const char* fname, const cmlib::SmlNode& root,
             const char* enc, bool bInsertIndent)
  throw(std::exception)
{
  FILE* f = NULL;
  static const char* szThisMethod = "XMLwriteFile()";

  f = fopen(fname, "wt");
  if (f==NULL)
  {
    std::string msg = std::string("Cannot create file ")+fname;
    cmlib_throw(szThisMethod, msg);
  }

  try {
    XMLwriteFile(f, root, enc, bInsertIndent);
  } catch(const std::exception&) {
    fclose(f);
    throw;
  }
  fclose(f);
}

void cmlib::
XMLwriteFile(FILE* f, const cmlib::SmlNode& root,
             const char* enc, bool bInsertIndent)
  throw(std::exception)
{
  if ( f==NULL ) f = stdout;
  std::stack<ParseNodeDescr> treeStack;

  // Print XML header. We use 8-bit encoding `enc', or the default `us ascii'
  fprintf(f, "<?xml version=\"1.0\"");
  if (enc && *enc)
    fprintf(f, " encoding=\"%s\"?>\n", normalizeXML(enc).c_str());
  else
    fprintf(f, "?>\n");

  // Insert root item & initialize level 0 iterator.
  treeStack.push(ParseNodeDescr(&root));

  // Indentation level
  unsigned int level = 0;

  while ( ! treeStack.empty() )
    {
      ParseNodeDescr* descr = &(treeStack.top());
      const cmlib::SmlNode* p = descr->node_;

      // Indent tag line
      if (bInsertIndent) putcN(f, ' ', level*2);

      const char* txtData = p->text();
      if ( txtData==NULL ) txtData = "";

      // Check for possible <![CDATA[ ... ]]> section
      if ( p->eqName("!CDATA") )
        {
          // Process <![CDATA[ ... ]]> section
          fprintf(f, "<![CDATA[");
          fprintf(f, "%s", txtData);
          fprintf(f, "]]>\n");
        }
      else
        {
          // Print tag and attributes
          fprintf(f, "<%s", normalizeXML(p->name(),true,true).c_str());
          cmlib::SmlNode::tAttributes::const_iterator it;
          for ( it=p->attributes_.begin(); it!=p->attributes_.end(); it++ )
            {
              fprintf(f, " %s=\"%s\"", normalizeXML((*it).first.c_str(),true,true).c_str(),
                      normalizeXML((*it).second.c_str()).c_str());
            }
          // Close tag line in a correct manner.
          if ( (p->nodeCount()>0) || (txtData && txtData[0]) )
            fprintf(f, ">\n");
          else
            fprintf(f, "/>\n");
          // Print text, if one present
          if ( txtData && txtData[0] )
            {
              if (bInsertIndent) putcN(f, ' ', level*2);
              fprintf(f, "%s\n", normalizeXML(txtData,true).c_str());
            }
        }

      // Now we will enter tag-close cycle
      while ( descr->pos_ >= descr->node_->children_.size() )
        {
          // Close tag, if needed. First check for CDATA.
          if ( ! p->eqName("!CDATA") )
            {
              const char* txtData = p->text();
              if ( (p->nodeCount()>0) || (txtData && txtData[0]) )
                {
                  if (bInsertIndent) putcN(f, ' ', level*2);
                  fprintf(f, "</%s>\n", normalizeXML(p->name(),true,true).c_str());
                }
            }
          // Remove the topmost item and decrease the indentation level.
          treeStack.pop();
          --level;
          if ( treeStack.empty() )
            break;
          descr = &(treeStack.top());
          p = descr->node_;
        }

      // Nothing to do if all tags were closed
      if ( treeStack.empty() )
        break;

      // Otherwise push next item, move through it and continue.
      treeStack.push(ParseNodeDescr(p->children_[descr->pos_]));
      descr->pos_++;
      // Increase the indentation level
      ++level;
    }
}

void cmlib::
XMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len, const SmlNode& root,
               const char* enc, osp_malloc_t dllm_malloc, bool bInsertIndent)
  throw(std::exception)
{
  cmlib::basestream f;
  data = NULL; len = 0;

  XMLwriteStream(f, root, enc, bInsertIndent);

  len = f.ncount();
  data = dllm_malloc(len+1); ((char*)data)[len] = 0;
  f.read(data, len);
}

void cmlib::
XMLwriteStream(cmlib::basestream& s, const SmlNode& root,
               const char* enc, bool bInsertIndent)
  throw(std::exception)
{
  std::stack<ParseNodeDescr> treeStack;
  textstream f;

  // Print XML header. We use 8-bit encoding `enc', or the default `us ascii'
  f << "<?xml version=\"1.0\"";
  if (enc && *enc)
    f.printf(" encoding=\"%s\"?>\n", normalizeXML(enc).c_str());
  else
    f << "?>\n";

  // Insert root item & initialize level 0 iterator.
  treeStack.push(ParseNodeDescr(&root));

  // Indentation level
  unsigned int level = 0;

  while ( ! treeStack.empty() )
    {
      ParseNodeDescr* descr = &(treeStack.top());
      const cmlib::SmlNode* p = descr->node_;

      // Indent tag line
      if (bInsertIndent) putcN(f, ' ', level*2);

      const char* txtData = p->text();
      if ( txtData==NULL ) txtData = "";

      // Check for possible <![CDATA[ ... ]]> section
      if ( p->eqName("!CDATA") )
        {
          // Process <![CDATA[ ... ]]> section
          f << "<![CDATA[";
          f << txtData;
          f << "]]>\n";
        }
      else
        {
          // Print tag and attributes
          f << "<" << normalizeXML(p->name(),true,true).c_str();
          cmlib::SmlNode::tAttributes::const_iterator it;
          for ( it=p->attributes_.begin(); it!=p->attributes_.end(); it++ )
            {
              f << " " << normalizeXML((*it).first.c_str(),true,true).c_str() <<
                "=\"" << normalizeXML((*it).second.c_str()).c_str() << "\"";
            }
          // Close tag line in a correct manner.
          if ( (p->nodeCount()>0) || (txtData && txtData[0]) )
            f << ">\n";
          else
            f << "/>\n";
          // Print text, if one present
          if ( txtData && txtData[0] )
            {
              if (bInsertIndent) putcN(f, ' ', level*2);
              f << normalizeXML(txtData,true).c_str() << "\n";
            }
        }

      // Now we will enter tag-close cycle
      while ( descr->pos_ >= descr->node_->children_.size() )
        {
          // Close tag, if needed. First check for CDATA.
          if ( ! p->eqName("!CDATA") )
            {
              const char* txtData = p->text();
              if ( (p->nodeCount()>0) || (txtData && txtData[0]) )
                {
                  if (bInsertIndent) putcN(f, ' ', level*2);
                  f << "</" << normalizeXML(p->name(),true,true).c_str() << ">\n";
                }
            }
          // Remove the topmost item and decrease the indentation level.
          treeStack.pop();
          --level;
          if ( treeStack.empty() )
            break;
          descr = &(treeStack.top());
          p = descr->node_;
        }

      // Nothing to do if all tags were closed
      if ( treeStack.empty() )
        break;

      // Otherwise push next item, move through it and continue.
      treeStack.push(ParseNodeDescr(p->children_[descr->pos_]));
      descr->pos_++;
      // Increase the indentation level
      ++level;
    }

  s.steal_from(f);
}


//-----------------------------------------------------------------
// Process data in SML pseudo-markup language

inline void writeString(FILE* f, const char* str)
{
  u_intp_ot n = str ? strlen(str) : 0;
  u_intp_ot c = fwrite(str, 1, n, f);
  if ( (fwrite("", 1, 1, f))!=1 || (c!=n) )
    cmlib_throw("SMLwriteFile", "Output error");
}

inline void writeLong(FILE* f, u_intp_ot x)
{
  char buf[64]; sprintf(buf, "%u", x);
  writeString(f, buf);
}

void cmlib::
SMLwriteFile(const char* fname, const cmlib::SmlNode& root, const char* enc/*=NULL*/)
  throw(std::exception)
{
  FILE* f = NULL;
  static const char* szThisMethod = "SMLwriteFile()";

  // We need a binary io mode
  f = fopen(fname, "wb");
  if (f==NULL)
  {
    std::string msg = std::string("Cannot create file ")+fname;
    cmlib_throw(szThisMethod, msg);
  }

  try {
    SMLwriteFile(f, root, enc);
  } catch(const std::exception&) {
    fclose(f);
    throw;
  }
  fclose(f);
}

void cmlib::
SMLwriteFile(FILE* f, const cmlib::SmlNode& root, const char* enc/*=NULL*/)
  throw(std::exception)
{
  if ( f==NULL ) f = stdout;
  std::stack<ParseNodeDescr> treeStack;

  if ( enc==NULL )
    enc = OSP_ENCODING;

  // Write SML header.
  writeString(f, SML_HEADER);
  // Write SML encoding data
  writeString(f, enc);

  // Insert root item & initialize level 0 iterator.
  treeStack.push(ParseNodeDescr(&root));

  while ( ! treeStack.empty() )
    {
      ParseNodeDescr* descr = &(treeStack.top());
      const cmlib::SmlNode* p = descr->node_;

      // Print tag and attributes
      writeString(f, p->name());
      // Save the number of attributes
      writeLong(f, p->attrCount());
      // And their name-value pairs
      cmlib::SmlNode::tAttributes::const_iterator it;
      for ( it=p->attributes_.begin(); it!=p->attributes_.end(); it++ )
        {
          writeString(f, (*it).first.c_str());
          writeString(f, (*it).second.c_str());
        }
      // Print text
      const char* txtData = p->text();
      if ( txtData==NULL ) txtData = "";
      writeString(f, txtData);

      // Save the number of subchildren
      writeLong(f, p->nodeCount());

      // Now we will enter tag-close cycle
      while ( descr->pos_ >= descr->node_->children_.size() )
        {
          // Remove the topmost item
          treeStack.pop();
          if ( treeStack.empty() )
            break;
          descr = &(treeStack.top());
          p = descr->node_;
        }

      // Nothing to do if all tags were closed
      if ( treeStack.empty() )
        break;

      // Otherwise push next item, move through it and continue.
      treeStack.push(ParseNodeDescr(p->children_[descr->pos_]));
      descr->pos_++;
    }
}

void cmlib::
SMLwriteBuffer(osp_void_ptr& data, u_intp_ot& len,
               const SmlNode& root, osp_malloc_t dllm_malloc,
               const char* enc/*=NULL*/)
  throw(std::exception)
{
  data = NULL; len = 0;
  cmlib::basestream buf;
  SMLwriteStream(buf, root, enc);

  len = buf.ncount();
  data = dllm_malloc(len+1); ((char*)data)[len] = 0;
  buf.read(data, len);
}

void cmlib::
SMLwriteStream(cmlib::basestream& s,
               const SmlNode& root, const char* enc/*=NULL*/)
  throw(std::exception)
{
  std::stack<ParseNodeDescr> treeStack;
  textstream buf;

  if ( enc==NULL )
    enc = OSP_ENCODING;

  // Write SML header.
  buf << SML_HEADER;
  buf.write("\0", 1);
  // Write SML encoding data
  buf << enc;
  buf.write("\0", 1);

  // Insert root item & initialize level 0 iterator.
  treeStack.push(ParseNodeDescr(&root));

  unsigned long i;

  while ( ! treeStack.empty() )
  {
    ParseNodeDescr* descr = &(treeStack.top());
    const cmlib::SmlNode* p = descr->node_;

    // Print tag and attributes
    buf << p->name(); buf.write("\0", 1);
    // Save the number of attributes
    i = p->attrCount();
    buf << i; buf.write("\0", 1);
    // And their name-value pairs
    cmlib::SmlNode::tAttributes::const_iterator it;
    for ( it=p->attributes_.begin(); it!=p->attributes_.end(); it++ )
    {
      buf<<(*it).first.c_str(); buf.write("\0", 1);
      buf<<(*it).second.c_str(); buf.write("\0", 1);
    }
    // Print text
    const char* txtData = p->text();
    if ( txtData==NULL ) txtData = "";
    buf << txtData; buf.write("\0", 1);

    // Save the number of subchildren
    i = p->nodeCount();
    buf << i; buf.write("\0", 1);

    // Now we will enter tag-close cycle
    while ( descr->pos_ >= descr->node_->children_.size() )
    {
      // Remove the topmost item
      treeStack.pop();
      if ( treeStack.empty() )
        break;
      descr = &(treeStack.top());
      p = descr->node_;
    }

    // Nothing to do if all tags were closed
    if ( treeStack.empty() )
      break;

    // Otherwise push next item, move through it and continue.
    treeStack.push(ParseNodeDescr(p->children_[descr->pos_]));
    descr->pos_++;
  }

  s.steal_from(buf);
}

//---------------------------------------------------------------------
void cmlib::
SMLreadFile(const char* fname, cmlib::SmlNode& root, std::string* enc, bool clean)
  throw(std::exception)
{
  if (clean)
    root.clear();
  FILE* f = NULL;
  static const char* szThisMethod = "SMLreadFile()";

  // We need a binary io mode
  f = fopen(fname, "rb");
  if (f==NULL)
  {
    std::string msg = std::string("Cannot open file ")+fname;
    cmlib_throw(szThisMethod, msg);
  }

  try {
    SMLreadFile(f, root, enc, clean);
  } catch(const std::exception&) {
    fclose(f); // cleanup now
    throw;
  }
  fclose(f);
}

static inline char* movePointer(char* p, u_intp_ot& s)
  throw(std::exception)
{
  if ( p==NULL ) return NULL;
  while ( *p )
  {
    if ( (s--) <= 1 ) {
      cmlib_throw("SMLreadBuffer()", "Unrecognized SML format: unexpected end of data");
    }
    ++p;
  }
  if (s) --s;
  return ++p;
}

struct ParseNodeDescrIn
{
  ParseNodeDescrIn(cmlib::SmlNode* node, u_intp_ot pos=0) :
    node_(node), pos_(pos) {}

  cmlib::SmlNode* node_;
  u_intp_ot pos_;
};


void cmlib::
SMLreadBuffer(osp_void_ptr buf, u_intp_ot len, cmlib::SmlNode& root,
              std::string* enc, bool clean)
  throw(std::exception)
{
  static const char* szThisMethod = "readSMLbuffer()";
  unsigned int nlines = 0;
  if (clean)
    root.clear();
  if ( buf==NULL || len==0 ) {
    cmlib_throw(szThisMethod, "Invalid arguments - NULL pointer");
  }
  char* str = (char*)buf;
  u_intp_ot pos = len;

  // Check SML header.
  if ( strcmp(SML_HEADER, str)!=0 ) {
    cmlib_throw(szThisMethod, "Unrecognized SML format: bad data header.");
  }

  // Skip header and go ahead
  str = movePointer(str, pos); ++nlines;

  // Read or skip SML encoding
  if ( enc )
      *enc = str;
  str = movePointer(str, pos); ++nlines;

  std::stack<ParseNodeDescrIn> treeStack;
  treeStack.push(ParseNodeDescrIn(&root));

  u_intp_ot attrCount;
  std::string attrName;
  std::string attrValue;

  do {
    // Read next item name and attribute count
    treeStack.top().node_->name(str);
    str = movePointer(str, pos);
    attrCount = atol(str);
    str = movePointer(str, pos);

    // Read the values of attributes
    while ( attrCount-- )
    {
      attrName = str;
      str = movePointer(str, pos);
      attrValue = str;
      str = movePointer(str, pos);
      treeStack.top().node_->setAttr(
        attrName.c_str(), attrValue.c_str());
    }
    nlines += 2;
    treeStack.top().node_->npos_line(nlines);
    treeStack.top().node_->npos_column((unsigned int)(str-(char*)buf));

    // Read the text value
    treeStack.top().node_->text(str);
    if ( *str ) ++nlines;
    str = movePointer(str, pos);

    // Read the `left children count' value
    treeStack.top().pos_ = atol(str);
    str = movePointer(str, pos);

    // Tag-close cycle
    while ( treeStack.top().pos_ == 0 )
    {
      treeStack.pop();
      if ( treeStack.empty() )
        break;
    }
    if ( treeStack.empty() )
      break;

    // Push next subitem, decrease left_ value.
    treeStack.top().pos_--;
    // Insert new node with strange name
    cmlib::SmlNode& tmpNode =
      treeStack.top().node_->node(" \a ");
    treeStack.push(ParseNodeDescrIn(&tmpNode));

  } while( ! treeStack.empty() );
}


static void
movePointer(FILE *f, char*& s, int& len, bool noExpand=false)
  throw(std::exception)
{
  const int expand = 16384;
  char *p = s;
  char *end = s + len;

  int nextChar;
  do {
    if ( (nextChar = fgetc(f)) != EOF )
    {
      if ( p == end )
      {
        if ( noExpand ) {
          osp_free(s); s = NULL;
          cmlib_throw("SMLreadFile()", "Unrecognized SML format: bad field length");
        }
        s = (char*)osp_realloc_ex(s, len+expand);
        if ( s==NULL )
          cmlib_throw("SMLreadFile()", "Out of memory");
        p = s + len;
        len += expand; end = s + len;
      }
      *p++ = char((unsigned char)nextChar);
    }
  } while ( nextChar && nextChar!=EOF );

  if ( nextChar )
    {
      osp_free(s); s = NULL;
      cmlib_throw("SMLreadFile()", "Unrecognized SML format: unexpected end of data");
    }
}


void cmlib::
SMLreadFile(FILE* f, cmlib::SmlNode& root, std::string* enc, bool clean)
  throw(std::exception)
{
  if ( f==NULL ) f = stdin;
  unsigned int nlines = 0;

  if (clean)
    root.clear();
  static const char* szThisMethod = "SMLreadFile()";

  int len = 16384;
  char* str = (char*)osp_malloc(len);

  // Check SML header.
  movePointer(f, str, len, true); ++nlines;
  if ( strcmp(SML_HEADER, str)!=0 )
    cmlib_throw(szThisMethod, "Unrecognized SML format: bad data header");

  // Read or skip SML encoding
  movePointer(f, str, len, true); ++nlines;
  if ( enc ) *enc = str;

  std::stack<ParseNodeDescrIn> treeStack;
  treeStack.push(ParseNodeDescrIn(&root));

  u_intp_ot attrCount;
  std::string attrName;

  do {
    // Read next item name and attribute count
    movePointer(f, str, len, true);
    treeStack.top().node_->name(str);
    movePointer(f, str, len, true);
    attrCount = atol(str);

    // Read the values of attributes
    while ( attrCount-- )
    {
      movePointer(f, str, len, true);
      attrName = str;
      movePointer(f, str, len);
      treeStack.top().node_->setAttr(attrName.c_str(), str);
    }
    nlines += 2;
    treeStack.top().node_->npos_line(nlines);

    // Read the text value
    movePointer(f, str, len);
    treeStack.top().node_->text(str);
    if ( *str ) ++nlines;

    // Read the `left children count' value
    movePointer(f, str, len, true);
    treeStack.top().pos_ = atol(str);

    // Tag-close cycle
    while ( treeStack.top().pos_ == 0 )
    {
      treeStack.pop();
      if ( treeStack.empty() )
        break;
    }
    if ( treeStack.empty() )
      break;

    // Push next subitem, decrease left_ value.
    treeStack.top().pos_--;
    // Insert new node with strange name
    cmlib::SmlNode& tmpNode =
      treeStack.top().node_->node(" \a ");
    treeStack.push(ParseNodeDescrIn(&tmpNode));

  } while( ! treeStack.empty() );

  if (str) osp_free(str);
}
