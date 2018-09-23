/* -*- C++ -*- */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <files.h>

osp_file_t osp_fopen(const char* path, const char* acc)
{
  int oflag = O_CREAT;
  while ( acc && *acc )
    {
      switch ( *acc )
        {
        case 'r':
          if ( oflag & O_WRONLY )
            {
              oflag = oflag & (~O_WRONLY);
              oflag |= O_RDWR;
            }
          else
            oflag |= O_RDONLY;
          break;
        case 'w':
          if ( oflag & O_RDONLY )
            {
              oflag = oflag & (~O_RDONLY);
              oflag |= O_RDWR;
            }
          else
            oflag |= O_WRONLY;
          break;
        case 't':
          if ( oflag & O_RDONLY )
            {
              oflag = oflag & (~O_RDONLY);
              oflag |= O_RDWR | O_TRUNC;
            }
          else
            oflag |= O_WRONLY | O_TRUNC;
          break;
        case 'a':
          oflag |= O_APPEND;
          break;
        }
      ++acc;
    }
  
  return (osp_file_t)open(path, oflag, 644);
}

void osp_fclose(osp_file_t h)
{
  close((int)h);
}

u_intp_ot
osp_fseek(osp_file_t h, intp_ot pos, int where)
{
  int whence;
  off_t off;
  switch (where)
    {
    case OSP_SEEK_SET:
      whence = SEEK_SET;
      break;
    case OSP_SEEK_CUR:
      whence = SEEK_CUR;
      break;
    case OSP_SEEK_END:
      whence = SEEK_END;
      break;
    default:
      whence = SEEK_SET;
    };
  off = lseek((int)h, (off_t)pos, whence);
  if ( off == (off_t)-1 )
    return (u_intp_ot)~0;
  return (u_intp_ot)off;
}

u_intp_ot
osp_fpos(osp_file_t h)
{
  off_t off = lseek((int)h, 0, SEEK_CUR);
  if ( off == (off_t)-1 )
    return (u_intp_ot)~0;
  return (u_intp_ot)off;
}

intp_ot osp_fread(osp_file_t h, void* buf, intp_ot len)
{
  return read((int)h, buf, len);
}

intp_ot osp_fwrite(osp_file_t h, const void* buf, intp_ot len)
{
  return write((int)h, buf, len);
}

intp_ot osp_pipe(osp_file_t* pin, osp_file_t* pout)
{
  int fildes[2];
  if ( pipe(fildes)!=0 )
    return -1;
  if ( pin )
    *pin = (osp_file_t)fildes[0];
  else
    close(fildes[0]);
  if ( pout )
    *pout = (osp_file_t)fildes[1];
  else
    close(fildes[1]);
  return 0;
}
