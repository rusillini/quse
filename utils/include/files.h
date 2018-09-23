/* -*- C++ -*- */

#ifndef __files_h__
#define __files_h__

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef intp_ot osp_file_t;
  
#define OSP_FILE_BAD ((osp_file_t)-1)

#define OSP_SEEK_SET 0
#define OSP_SEEK_CUR 1
#define OSP_SEEK_END 2
  
  osp_file_t osp_fopen(const char* path, const char* acc);
  
  void osp_fclose(osp_file_t h);
  
  u_intp_ot osp_fseek(osp_file_t h, intp_ot pos, int where);
  
  u_intp_ot osp_fpos(osp_file_t h);
  
  intp_ot osp_fread(osp_file_t h, void* buf, intp_ot len);
  
  intp_ot osp_fwrite(osp_file_t h, const void* buf, intp_ot len);
  
  intp_ot osp_pipe(osp_file_t* pin, osp_file_t* pout);
  
#ifdef __cplusplus
}
#endif

#endif /* __files_h__ */
