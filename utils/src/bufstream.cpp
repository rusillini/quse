#include <config.h>
#define CMLIB_DISABLE_IOSTREAM
#include <bufstream.h>
#include <assert.h>
#include <string.h>

cmlib::basestream::
basestream(u_intp_ot chunkSize)
{
  assert(chunkSize>0);
  defChunkSize_ = chunkSize;

  pLastChunk_ = pFirstChunk_ = NULL;
  nReadPos_ = nWritePos_ = 0;
  bytesCount_ = 0;
}

cmlib::basestream::
~basestream()
{
  cleanup();
}

//----------------------------------------------------------
void cmlib::basestream::
add_chunk(u_intp_ot len)
{
  chunk_t *pChunk;
  if ( len==0 ) len = defChunkSize_;
  pChunk = (chunk_t*)malloc( sizeof(chunk_t)+len );
  pChunk->size = len;
  pChunk->next = NULL;
  if ( pLastChunk_ )
    pLastChunk_->next = pChunk;
  pLastChunk_ = pChunk;
  if ( pFirstChunk_==NULL )
    pFirstChunk_ = pChunk;
}

void cmlib::basestream::
remove_chunk()
{
  chunk_t *pChunk = pFirstChunk_;
  if ( pChunk )
    {
      if ( pFirstChunk_==pLastChunk_ )
        pLastChunk_ = NULL;
      pFirstChunk_ = pFirstChunk_->next;
      free(pChunk);
    }
}

//----------------------------------------------------------
u_intp_ot cmlib::basestream::
write(const void* data, u_intp_ot count)
{
  if ( data==NULL || count==0 )
    return 0;
  if ( pLastChunk_==NULL )
    add_chunk();

  bytesCount_ += count;
  u_intp_ot nRetVal = count;
  while ( count )
    {
      u_intp_ot chunkSize = pLastChunk_->size;
      if ( nWritePos_ >= chunkSize )
        {
          nWritePos_ = 0;
          add_chunk();
        }
      u_intp_ot toCopy = (count > chunkSize-nWritePos_) ?
        (chunkSize-nWritePos_) : count;
      byte_t *pWriteAddr = (byte_t*)(pLastChunk_)+sizeof(chunk_t);
      memcpy(pWriteAddr+nWritePos_, data, toCopy);
      count -= toCopy;
      data = (byte_t*)data + toCopy;
      nWritePos_ += toCopy;
    }
  return nRetVal;
}

u_intp_ot cmlib::basestream::
read(void* data, u_intp_ot count)
{
  if ( data==NULL || count==0 )
    return 0;
  if ( pFirstChunk_==NULL || bytesCount_==0 )
    return 0;

  count = (count>bytesCount_) ? bytesCount_ : count;
  u_intp_ot nRetVal = count;
  bytesCount_ -= count;
  while ( count )
    {
      u_intp_ot chunkSize = pFirstChunk_->size;
      u_intp_ot toCopy = (count>chunkSize-nReadPos_) ?
        (chunkSize-nReadPos_) : count;
      const byte_t *pReadAddr = (byte_t*)(pFirstChunk_)+sizeof(chunk_t);
      memcpy(data, pReadAddr+nReadPos_, toCopy);
      count -= toCopy;
      data = (byte_t*)data + toCopy;
      nReadPos_ += toCopy;
      if ( nReadPos_ >= chunkSize )
        {
          nReadPos_ = 0;
          remove_chunk();
        }
    }
  return nRetVal;
}

u_intp_ot cmlib::basestream::
write_bmk(const void* data, u_intp_ot count,
          const cmlib::basestream::bookMark& bmk)
{
  if ( data==NULL || count==0 )
    return 0;
  if ( bmk.chunk==NULL )
    return 0;
  if ( bmk.pos>=bmk.chunk->size )
    return 0;

  u_intp_ot nRetVal = count;
  chunk_t* pChunk = bmk.chunk;
  u_intp_ot pos = bmk.pos;
  while ( count )
    {
      if ( pChunk==NULL )
        return nRetVal-count;
      u_intp_ot chunkSize = pChunk->size;
      u_intp_ot toCopy = (count>chunkSize-pos) ?
        (chunkSize-pos) : count;
      byte_t *pWriteAddr = (byte_t*)(pChunk)+sizeof(chunk_t);
      memcpy(pWriteAddr+pos, data, toCopy);
      count -= toCopy;
      data = (byte_t*)data + toCopy;
      pos += toCopy;
      if ( pos >= chunkSize )
        {
          pos = 0;
          pChunk = pChunk->next;
        }
    }
  return nRetVal;
}


void cmlib::basestream::
steal_from(cmlib::basestream& s)
{
  cleanup();
  pFirstChunk_ = s.pFirstChunk_; s.pFirstChunk_ = NULL;
  pLastChunk_ = s.pLastChunk_; s.pLastChunk_ = NULL;
  nReadPos_ = s.nReadPos_; s.nReadPos_ = 0;
  nWritePos_ = s.nWritePos_; s.nWritePos_ = 0;
  bytesCount_ = s.bytesCount_; s.bytesCount_ = 0;
  defChunkSize_ = s.defChunkSize_;
}

void cmlib::basestream::
append_from(cmlib::basestream& s)
{
  if ( s.ncount() < 1 )
    return; // No-op case

  if ( bytesCount_ < 1 )
    {
      // Trivial case
      steal_from(s);
      return;
    }

  // "Shrink" storage for the last chunk
  pLastChunk_->size = nWritePos_;

  if (s.nReadPos_ > 0 )
    {// we have to copy the first segment
      int32_ot toCopy = s.pFirstChunk_->size - s.nReadPos_;
      if ( toCopy > 0 )
        {
          add_chunk(toCopy);
          byte_t *m_from = (byte_t*)(s.pFirstChunk_)+sizeof(chunk_t)+s.nReadPos_;
          byte_t *m_to = (byte_t*)(pLastChunk_)+sizeof(chunk_t);
          memcpy(m_to, m_from, toCopy);
          nWritePos_ = toCopy;
        }
      s.nReadPos_ = 0;
      s.remove_chunk();
    }

  if ( s.pFirstChunk_ )
    {
      pLastChunk_->next = s.pFirstChunk_;
      pLastChunk_ = s.pLastChunk_;
      nWritePos_ = s.nWritePos_;
    }

  s.pFirstChunk_ = NULL;
  s.pLastChunk_ = NULL;
  s.nReadPos_ = 0;
  s.nWritePos_ = 0;
  s.bytesCount_ = 0;
}

//  ��������� �� ������ �������� �����
const void* cmlib::basestream::raw_ptr()
{
  while (true) {
    if ( pFirstChunk_==NULL )
      return NULL;
    if ( nReadPos_ < pFirstChunk_->size )
      break;
    nReadPos_ = 0;
    remove_chunk();
  }
  const byte_t *pReadAddr = (byte_t*)(pFirstChunk_)+sizeof(chunk_t);
  return (pReadAddr+nReadPos_);
}


//----------------------------------------------------------
int cmlib::textstream::
printf(const char *format, ...)
{
  va_list vl; char buf[4096];
  va_start(vl, format);
  int n = vsprintf(buf, format, vl);
  va_end(vl);
  buf[sizeof(buf)-1] = 0;
  this->write(buf, strlen(buf));
  return n;
}

cmlib::textstream&
operator<<(cmlib::textstream& bs, long dt)
{
  char buf[32]; int c = sprintf(buf, "%ld", dt);
  u_intp_ot count = bs.write(buf, c);
  if (u_intp_ot(c)!=count) {
    cmlib_throw("cmlib::textstream::operator<<", "out of memory");
  }
  return bs;
}
cmlib::textstream&
operator<<(cmlib::textstream& bs, unsigned long dt)
{
  char buf[32]; int c = sprintf(buf, "%lu", dt);
  u_intp_ot count = bs.write(buf, c);
  if (u_intp_ot(c)!=count)
    cmlib_throw("cmlib::textstream::operator<<", "out of memory");
  return bs;
}

// End Of File
