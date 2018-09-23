#include <config.h>
#define CMLIB_DISABLE_IOSTREAM
#include <encoding.h>

cmlib::EncodingHandler::
EncodingHandler(const char* tablePath/*=NULL*/)
{
  native_conv_ = NULL;
  addCodeTables(tablePath);
  setNative(NULL);
}

void cmlib::EncodingHandler::
cleanup()
{
  native_conv_ = NULL;
  tConvTables::iterator it;
  for ( it=all_conv_.begin(); it!=all_conv_.end(); it++ )
    {
      delete (*it);
      *it = NULL;
    }
  all_conv_.clear();
  find_conv_.clear();
}


inline void
parseChar(cmlib::EncodingHandler::tConvData& d, const char* s)
{
  cmlib::EncodingHandler::byte_t byte;
  cmlib::EncodingHandler::word_t word;
  
  byte = 0;
  while ( *s && *s>='0' && *s<='9' )
    {
      byte *= 10;
      byte += *s - '0';
      ++s;
    }
  
  while ( *s && (*s<'0' || *s>'9') )
    ++s;
  
  if ( *s=='\0' )
    return;
  
  word = 0;
  while ( *s && *s>='0' && *s<='9' )
    {
      word *= 10;
      word += *s - '0';
      ++s;
    }
  
  d.ft[word] = byte;
  d.rt[byte] = word;
}

inline void
initForwardTable(cmlib::EncodingHandler::tReverseTable& rt)
{
  unsigned int i;
  for ( i=0; i<=127; ++i )
    rt[i] = (unsigned char)i;
  
  for ( i=128; i<256; ++i )
    rt[i] = (unsigned char)'?';
}


bool cmlib::EncodingHandler::
addCodeTables(const char* tablePath)
{
  if ( tablePath==NULL )
    return false;
  
  FILE* f = fopen(tablePath, "rt");
  if ( !f ) return false;
  
  std::list<std::string> encNames;
  std::list<std::string>::iterator encNamesIt;
  tConvData *encData = NULL;
  char buf[256];
  
  while ( !feof(f) && !ferror(f) )
    {
      if ( fgets(buf, sizeof(buf), f) == NULL )
        break;
      int len = strlen(buf);
      if ( len>0 && (buf[len-1]=='\n' || buf[len-1]=='\r') )
        buf[len-1] = '\0';
      if ( len>1 && (buf[len-2]=='\n' || buf[len-2]=='\r') )
        buf[len-2] = '\0';
      
      switch ( buf[0] )
        {
        case '@':          
          if ( encData )
            {
              all_conv_.push_back(encData);
              for ( encNamesIt=encNames.begin();
                    encNamesIt!=encNames.end(); encNamesIt++ )
                {
                  find_conv_[*encNamesIt] = encData;
                }
            }
          encData = new tConvData;
          initForwardTable(encData->rt);
          encNames.clear();
          break;
          
        case 'C': case 'c': 
          if ( encData==NULL )
            continue;
          parseChar(*encData, buf+1);
          break;
          
        case 'N': case 'n': 
          encNames.push_back(buf+1);
          break;
        }; 
    }
  fclose(f);
  
  if ( encData )
    {
      all_conv_.push_back(encData);
      for ( encNamesIt=encNames.begin();
            encNamesIt!=encNames.end(); encNamesIt++ )
        {
          find_conv_[*encNamesIt] = encData;
        }
      return true;
    }
  
  return false;
}


bool cmlib::EncodingHandler::
writeCodeTables(const char* tablePath)
{
  if ( tablePath==NULL )
    return false;
  
  FILE* f = fopen(tablePath, "wt");
  if ( !f ) return false;
  
  tConvTables::const_iterator it;
  tFindConvTables::const_iterator n_it;
  
  for ( it=all_conv_.begin(); it!=all_conv_.end(); it++ )
    {
      fprintf(f, "@\n");
      for ( n_it=find_conv_.begin(); n_it!=find_conv_.end(); n_it++ )
        {
          if ( (*n_it).second == (*it) )
            fprintf(f, "N%s\n", (*n_it).first.c_str());
        }
      
      for ( int i=0; i<256; ++i )      
        fprintf(f, "C%d\t%d\n", i, int((*it)->rt[i]));
    }
  return true;
}

void cmlib::EncodingHandler::
setNative(const char* enc/*=NULL*/)
{
  if ( enc==NULL ) enc = "native";
  tFindConvTables::const_iterator it = find_conv_.find(enc);
  if ( it!=find_conv_.end() )
    native_conv_ = (*it).second;
}

bool cmlib::EncodingHandler::
hasEncoding(const char* name)
{
  if ( strcmp(name, "utf-8")==0 )
    return true;
  
  if ( findConv(name) )
    return true;
  return false;
}


//----------------------------------------------------------------------
inline int xStrLen(const unsigned char* s)
{
  if ( s==NULL )
    return 0;
  const unsigned char* p = s;
  while ( *p ) ++p;
  return (p - s);
}
inline int xStrLen(const unsigned short* s)
{
  if ( s==NULL )
    return 0;
  const unsigned short* p = s;
  while ( *p ) ++p;
  return (p - s);
}


bool cmlib::EncodingHandler::
fillWideArray(int to[256], const char* enc)
{
  if ( to==NULL )
    return false;
  
  tConvData* dt = findConv(enc);
  if ( dt==NULL )
    return false;
  
  for ( int i=0; i<256; ++i )
    to[i] = dt->rt[i];
  
  return true;
}

void cmlib::EncodingHandler::
plainToWide(const char x_from[], cmlib::EncodingHandler::word_t to[],
            const char* enc/*=NULL*/, int len/*=-1*/)
{
  byte_t *from = (byte_t*)x_from;
  if ( from==NULL || to==NULL )
    {
      if ( to ) to[0] = 0;
      return;
    }
  
  tConvData* dt = findConv(enc);
  if ( len<0 ) len = xStrLen(from);
  
  if ( dt )
    {
      for ( int i=0; i<=len; ++i )
        to[i] = dt->rt[from[i]];
    }
  else
    {
      for ( int i=0; i<=len; ++i )
        to[i] = (from[i]<=127) ? from[i] : '?';
    }
}

void cmlib::EncodingHandler::
wideToPlain(const cmlib::EncodingHandler::word_t from[], char x_to[],
            const char* enc/*=NULL*/, int len/*=-1*/)
{
  byte_t *to = (byte_t*)x_to;
  if ( from==NULL || to==NULL )
    {
      if ( to ) to[0] = 0;
      return;
    }
  
  tConvData* dt = findConv(enc);
  if ( len<0 ) len = xStrLen(from);
  tForwardTable::const_iterator it;
  
  if ( dt )
    {
      for ( int i=0; i<=len; ++i )
        {
          it = dt->ft.find(from[i]);
          if ( it==dt->ft.end() )
            to[i] = '?';
          else
            to[i] = (*it).second;
        }
    }
  else
    {
      for ( int i=0; i<=len; ++i )
        to[i] = (from[i]<=127) ? char(from[i]) : '?';
    }
}


void cmlib::EncodingHandler::
plainToPlain(const char* x_from, std::string& to,
             const char* encFrom, const char* encTo/*=NULL*/)
{
  byte_t *from = (byte_t*)x_from;
  if ( from==NULL )
    {
      to.resize(0);
      return;
    }
  if ( encFrom == encTo )
    {
      to = x_from;
      return;
    }
  if ( encFrom && encTo && strcmp(encFrom, encTo)==0 )
    {
      to = x_from;
      return;
    }
  if ( encFrom && strcmp(encFrom, "utf-8")==0 )
    {
      utf8ToPlain(x_from, to, encTo);
      return;
    }
  if ( encTo && strcmp(encTo, "utf-8")==0 )
    {
      plainToUtf8(x_from, to, encTo);
      return;
    }
  
  tConvData* dtFrom = findConv(encFrom);
  tConvData* dtTo = findConv(encTo);
  
  int len = xStrLen(from);
  to.resize(len);
  
  if ( dtFrom==NULL || dtTo==NULL )
    {
      while ( len-- > 0 )
        {
          to[len] = (from[len]<=127) ? char(from[len]) : '?';
        }
      return;
    }
  
  tForwardTable::const_iterator it;
  while ( len-- > 0 )
    {
      word_t code = (*dtFrom).rt[from[len]];
      it = (*dtTo).ft.find(code);
      if ( it==(*dtTo).ft.end() )
        to[len] = '?';
      else
        {
          to[len] = char((*it).second);
        }
    }
}

void cmlib::EncodingHandler::
plainToPlain(const char* x_from, std::string& to,
             const cmlib::EncodingHandler::tConvData* dtFrom,
             const cmlib::EncodingHandler::tConvData* dtTo, int len/*=-1*/)
{
  byte_t *from = (byte_t*)x_from;
  if ( from==NULL )
    {
      to.resize(0);
      return;
    }
  if ( dtFrom==dtTo )
    {
      to = x_from;
      return;
    }
  if ( dtFrom==NULL )
    {
      utf8ToPlain(x_from, to, dtTo, len);
      return;
    }
  if ( dtTo==NULL )
    {
      plainToUtf8(x_from, to, dtFrom, len);
      return;
    }
  
  if (len<0) len = xStrLen(from);
  to.resize(len);
  
  if ( dtFrom==NULL || dtTo==NULL )
    {
      while ( len-- > 0 )
        {
          to[len] = (from[len]<=127) ? char(from[len]) : '?';
        }
      return;
    }
  
  tForwardTable::const_iterator it;
  while ( len-- > 0 )
    {
      word_t code = (*dtFrom).rt[from[len]];
      it = (*dtTo).ft.find(code);
      if ( it==(*dtTo).ft.end() )
        to[len] = '?';
      else
        {
          to[len] = char((*it).second);
        }
    }
}


//----------------------------------------------------------------------
static const cmlib::EncodingHandler::byte_t
gUTFBytes[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

static const cmlib::EncodingHandler::dword_t
gUTFOffsets[6] = { 0, 0x3080, 0xE2080, 0x3C82080, 0xFA082080, 0x82082080 };

static const cmlib::EncodingHandler::byte_t gFirstByteMark[7] = {
  0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};
//----------------------------------------------------------------------


void cmlib::EncodingHandler::
utf8ToPlain(const char* from, std::string& to,
            const cmlib::EncodingHandler::tConvData* dt,
            int len/*=-1*/)
{
  if ( from==NULL )
    {
      to = "";
      return;
    }
  
  const byte_t* srcPtr = (const byte_t*)from;
  if ( len<0 ) len = xStrLen(srcPtr);
  const byte_t* srcEnd = ((const byte_t*)from) + len;
  unsigned long pos = 0;
  
  to.resize(len);
  tForwardTable::const_iterator it;
  
  while (srcPtr < srcEnd)
    {
      const byte_t firstByte = *srcPtr;
      if ( firstByte <= 127 )
        {
          to[pos++] = wchar_t(firstByte);
          srcPtr++;
          continue;
        }
      
      const unsigned int trailingBytes = gUTFBytes[firstByte];
      
      
      dword_t tmpVal = 0;
      switch(trailingBytes)
        {
        case 5 : tmpVal += *srcPtr++; tmpVal <<= 6;
        case 4 : tmpVal += *srcPtr++; tmpVal <<= 6;
        case 3 : tmpVal += *srcPtr++; tmpVal <<= 6;
        case 2 : tmpVal += *srcPtr++; tmpVal <<= 6;
        case 1 : tmpVal += *srcPtr++; tmpVal <<= 6;
        case 0 : tmpVal += *srcPtr++;
          break;
        };
      tmpVal -= gUTFOffsets[trailingBytes];
      
      if ( dt==NULL )
        {
          to[pos++] = '?';
          continue;
        }
      
      
      if ( !(tmpVal & 0xFFFF0000) )
        {
          it = dt->ft.find(word_t(tmpVal));
          if ( it==dt->ft.end() )
            to[pos++] = '?';
          else
            to[pos++] = char((*it).second);
        }
      else
        {
           to[pos++] = '?';
        }
    }
  to.resize(pos);
}


void cmlib::EncodingHandler::
plainToUtf8(const char* x_from, std::string& to,
            const cmlib::EncodingHandler::tConvData* dt,
            int len/*=-1*/)
{
  if ( x_from==NULL )
    {
      to = "";
      return;
    }
  
  const byte_t *from = (const byte_t*) x_from;
  if (len<0) len = xStrLen(from);
  byte_t symBuf[7];
  
  for ( int i=0; i<=len; ++i )
    {
      if ( from[i]=='\0' )
        continue;
      if ( from[i] <= 127 )
        {
          to.append(1, from[i]);
          continue;
        }
      if ( dt==NULL )
        {
          to.append(1, '?');
          continue;
        }
      dword_t curVal = dt->rt[from[i]];
      
      
      unsigned int encodedBytes;
      if (curVal < 0x80)
        encodedBytes = 1;
      else if (curVal < 0x800)
        encodedBytes = 2;
      else if (curVal < 0x10000)
        encodedBytes = 3;
      else if (curVal < 0x200000)
        encodedBytes = 4;
      else if (curVal < 0x4000000)
        encodedBytes = 5;
      else if (curVal <= 0x7FFFFFFF)
        encodedBytes = 6;
      else
        {
          to.append(1, '?');
          continue;
        }
      
      symBuf[encodedBytes] = 0;
      byte_t *outPtr = symBuf + encodedBytes;
      switch(encodedBytes)
        {
        case 6 : *--outPtr = byte_t((curVal | 0x80UL) & 0xBFUL);
          curVal >>= 6;
        case 5 : *--outPtr = byte_t((curVal | 0x80UL) & 0xBFUL);
          curVal >>= 6;
        case 4 : *--outPtr = byte_t((curVal | 0x80UL) & 0xBFUL);
          curVal >>= 6;
        case 3 : *--outPtr = byte_t((curVal | 0x80UL) & 0xBFUL);
          curVal >>= 6;
        case 2 : *--outPtr = byte_t((curVal | 0x80UL) & 0xBFUL);
          curVal >>= 6;
        case 1 : *--outPtr = byte_t(curVal | gFirstByteMark[encodedBytes]);
        }
      to.append((const char*)symBuf);
    }
}

cmlib::EncodingHandler::tConvData*
cmlib::EncodingHandler::
findConv(const char* enc)
{
  tConvData* dt = NULL;
  if ( enc==NULL )
    dt = native_conv_;
  else
    {
      tFindConvTables::const_iterator it = find_conv_.find(enc);
      if ( it!=find_conv_.end() )
        dt = (*it).second;
    }
  if ( !dt )
    {
      fseek(stderr, 0, SEEK_END);
      fprintf(stderr, "(cmlib) Warning: unknown encoding `%s'\n",
              enc ? enc : "native");
    }
  return dt;
}
