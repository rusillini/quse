/* -*- C++ -*- */
#include <config.h>
#include <restart.h>

int copyfile(int fromfd, int tofd) {
   char *bp;
   char buf[BLKSIZE];
   int bytesread, byteswritten;
   int totalbytes = 0;
   
   for( ; ; ) {
      while (((bytesread = read(fromfd, buf, BLKSIZE)) == -1) &&
             (errno == EINTR)) ;
      if (bytesread <= 0)
        break;
      bp = buf;
      while (bytesread > 0) {
         while(((byteswritten = write(tofd, bp, bytesread)) == -1) && 
               (errno == EINTR));
         if (byteswritten <= 0)
           break;
         totalbytes += byteswritten;
         bytesread -= byteswritten;
         bp += byteswritten;
      }
      if(byteswritten == -1)
        break;
   }
   return totalbytes;
}

ssize_t r_read(int fd, void *buf, size_t size) {
   ssize_t retval;
   
   while(retval = read(fd, buf, size), retval == -1 && errno == EINTR);
   return retval;
}

ssize_t r_write(int fd, void *buf, size_t size) {
   char *bufp;
   size_t bytestowrite;
   size_t byteswritten;
   size_t totalbytes;
   
   for(bufp = buf, bytestowrite = size, totalbytes = 0;
       bytestowrite > 0;
       bufp += byteswritten, bytestowrite -= byteswritten) {
         byteswritten = write(fd, bufp, bytestowrite);
         if((byteswritten == -1) && (errno != EINTR))
           return -1;
         if(byteswritten == -1)
           byteswritten = 0;
         totalbytes += byteswritten;
   }
   
   return totalbytes;
}

size_t readblock(int fd, void *buf, size_t size) {
   char *bufp;
   size_t bytestoread;
   size_t bytesread;
   size_t totalbytes;
   
   for(bufp = buf, bytestoread = size, totalbytes = 0;
       bytestoread > 0;
       bufp += bytesread, bytestoread -= bytesread) {
     bytesread = read(fd, bufp, bytestoread);
     if ((bytesread == 0) && (totalbytes == 0))
       return 0;
     if (bytesread == 0) {
       errno = EINVAL;
       return -1;
     }
     if ((bytesread == -1) && (errno != EINTR))
       return -1;
     if (bytesread == -1)
       bytesread = 0;
     totalbytes += bytesread;
   }
   return totalbytes;
}

int r_close(int fd) {
   int retval;
   
   while(retval = close(fd), retval == -1 && errno == EINTR);
   return retval;
}

pid_t r_wait(int *stat_loc) {
  int retval;
  while(((retval = wait(stat_loc)) == -1) && (errno == EINTR));
  return retval;
}

           
