#ifndef __cmlib_bufstream_h__
#define __cmlib_bufstream_h__

#include "config.h"
#include <except.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define bufstream_badPos (~0)

namespace cmlib {

  //----------------------------------------------------------
  // A simple data buffer
  class basestream
  {
  private:
    // Disable copy-constructor & assignment operator
    basestream(const basestream&) {}
    basestream& operator=(const basestream&) { return *this; }

  public:
    // Single-byte item
    typedef unsigned char byte_t;
    // Data chunk
    struct chunk_t
    {
      chunk_t* next;
      u_intp_ot size;
    };

  protected:
    chunk_t *pFirstChunk_, *pLastChunk_;
    u_intp_ot nReadPos_, nWritePos_;
    u_intp_ot bytesCount_;
    u_intp_ot defChunkSize_;

  public:
    class bookMark
    {
    public:
      chunk_t *chunk;
      u_intp_ot pos;

      bookMark() : chunk(NULL), pos(0) {}
    };

  protected:
    void* malloc(u_intp_ot n)
    { return ::malloc((size_t)n); }
    void free(void *p)
    { ::free(p); }

    void add_chunk(u_intp_ot len=0);
    void remove_chunk();

  public:
    basestream(u_intp_ot chunkSize=16384-sizeof(chunk_t));
    virtual ~basestream();
    void steal_from(basestream& s);
    void append_from(basestream& s);

    void cleanup()
    {
      while ( pFirstChunk_ ) remove_chunk();
      nReadPos_ = nWritePos_ = 0;
      bytesCount_ = 0;
    }

    u_intp_ot write(const void* data, u_intp_ot count);
    u_intp_ot read(void* data, u_intp_ot count);

    u_intp_ot ncount()
    { return bytesCount_; }

    void get_bmk(bookMark& bmk)
    {
      bmk.chunk = pLastChunk_;
      bmk.pos = nWritePos_;
    }
    u_intp_ot write_bmk(const void* data, u_intp_ot count,
                        const bookMark& bmk);


    const void* raw_ptr();

    u_intp_ot raw_size()
    {
      if ( bytesCount_ )
	{
	  u_intp_ot len = pFirstChunk_->size - nReadPos_;
	  return (len > bytesCount_) ? bytesCount_ : len;
	}
      return 0;
    }

    void raw_next()
    {
      if ( bytesCount_ )
	{
	  bytesCount_ -= raw_size();
	  remove_chunk();
	  nReadPos_ = 0;
	}
    }
  };


  //----------------------------------------------------------

  class bufstream : public basestream
  {
  public:
    bufstream(size_t chunkSize=16384-sizeof(chunk_t)) :
      basestream(chunkSize)
    {}
  };

  //----------------------------------------------------------
  // A text output-oriented buffer
  class textstream : public basestream
  {
  public:
    textstream(size_t chunkSize=16384-sizeof(chunk_t)) :
      basestream(chunkSize)
    {}

    int printf(const char *format, ...);
    void pzero() { write("", 1); }
  };

} // namespace cmlib


template<class O>
cmlib::bufstream&
operator<<(cmlib::bufstream& bs, const O& dt)
{
  size_t count = bs.write(&dt, sizeof(O));
  if (count!=sizeof(O))
    cmlib_throw("cmlib::bufstream::operator<<", "out of memory");
  return bs;
}

template<class O>
cmlib::bufstream&
operator>>(cmlib::bufstream& bs, O& dt )
{
  size_t count = bs.read(&dt, sizeof(O));
  if (count!=sizeof(O)) {
    cmlib_throw("cmlib::bufstream::operator>>", "out of data");
  }
  return bs;
}

inline cmlib::textstream&
operator<<(cmlib::textstream& bs, const char* s)
{
  if (s==NULL || *s=='\0') return bs;
  u_intp_ot x = strlen(s), count = bs.write(s, x);
  if (x!=count) {
    cmlib_throw("cmlib::textstream::operator<<", "out of memory");
  }
  return bs;
}
cmlib::textstream&
operator<<(cmlib::textstream& bs, long dt);
cmlib::textstream&
operator<<(cmlib::textstream& bs, unsigned long dt);


#endif // __cmlib_bufstream_h__
