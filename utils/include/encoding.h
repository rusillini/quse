#ifndef __cmlib_encoding_h__
#define __cmlib_encoding_h__

#include "config.h"
#define CMLIB_DISABLE_IOSTREAM
#include "cmtypes.h"
#include <stdio.h>
#include <string.h>


namespace cmlib {

  class EncodingHandler
  {
  public:
    typedef u_int16_ot word_t;
    typedef u_int32_ot dword_t;
    typedef unsigned char byte_t;

    typedef std::map<word_t, byte_t> tForwardTable;
    typedef word_t tReverseTable[256];
    typedef struct {
      tForwardTable ft;
      tReverseTable rt;
    } tConvData;

    typedef std::map<std::string, tConvData*> tFindConvTables;
    typedef std::list<tConvData*> tConvTables;

  protected:
    tConvTables all_conv_;
    tFindConvTables find_conv_;
    tConvData* native_conv_;

  public:
    EncodingHandler(const char* tablePath=NULL);

    virtual ~EncodingHandler()
    {
      cleanup();
    }

    bool addCodeTables(const char* tablePath);
    void cleanup();
    void setNative(const char* enc=NULL);
    bool writeCodeTables(const char* tablePath);
    bool hasEncoding(const char* name);
    bool hasCodeTables() const
    {
      return ! ( all_conv_.empty() || find_conv_.empty() );
    }

    const tConvData* getCodingMagic(const char* name)
    {
      return findConv(name);
    }

    void plainToWide(const char from[], word_t to[], const char* enc=NULL, int len=-1);
    void wideToPlain(const word_t from[], char to[], const char* enc=NULL, int len=-1);

    bool fillWideArray(int to[256], const char* enc);

    void plainToPlain(const char* from, std::string& to,
                      const char* encFrom, const char* encTo=NULL);
    std::string plainToPlain(const char* from, const char* encFrom, const char* encTo=NULL)
    {
      std::string to;
      plainToPlain(from, to, encFrom, encTo);
      return to;
    }
    void plainToPlain(const char* from, std::string& to,
                      const tConvData* dtFrom, const tConvData* dtTo,
                      int len=-1);

    void utf8ToPlain(const char* from, std::string& to, const tConvData* dt, int len=-1);
    void utf8ToPlain(const char* from, std::string& to, const char* enc=NULL, int len=-1)
    {
      tConvData* dt = findConv(enc);
      utf8ToPlain(from, to, dt, len);
    }
    std::string utf8ToPlain(const char* from, const char* enc=NULL)
    {
      std::string to;
      utf8ToPlain(from, to, enc);
      return to;
    }

    void plainToUtf8(const char* from, std::string& to, const tConvData* dt, int len=-1);
    void plainToUtf8(const char* from, std::string& to, const char* enc=NULL, int len=-1)
    {
      tConvData* dt = findConv(enc);
      plainToUtf8(from, to, dt, len);
    }
    std::string plainToUtf8(const char* from, const char* enc=NULL)
    {
      std::string to;
      plainToUtf8(from, to, enc);
      return to;
    }

  protected:
    tConvData* findConv(const char* enc);
  };

}; // namespace cmlib


#endif // __cmlib_encoding_h__
