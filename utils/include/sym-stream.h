/* -*- C++ -*- */

#ifndef __sym_stream_h__
#define __sym_stream_h__

#include <config.h>

#define OSPPOLLIN  0x0001
#define OSPPOLLOUT 0x0002
#define OSPPOLLERR 0x0004
#define OSPPOLLHUP 0x0008

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef intp_ot osp_stream_t;
  typedef void* osp_pollh_t;
  
#define OSP_STREAM_BAD ((osp_stream_t)~0)
  
  struct osp_pollfd {
    osp_stream_t fd;
    short events;
    short revents;
  };
  
  typedef osp_stream_t (*stream_listen_ptr)(const char* addr);
  
  typedef osp_stream_t (*stream_accept_ptr)(osp_stream_t lsn);
  
  typedef osp_stream_t (*stream_connect_ptr)(const char* addr);

  typedef short (*stream_poll1_ptr)(osp_stream_t fd, short events, intp_ot timeout);
  
  typedef osp_pollh_t (*stream_prepp_ptr) (struct osp_pollfd cnc[], u_int32_ot ncnc);  
  
  typedef void (*stream_releasep_ptr)(osp_pollh_t);
  
  typedef intp_ot (*stream_cancelp_ptr)(osp_pollh_t);
  
  typedef intp_ot (*stream_poll_ptr)(osp_pollh_t h, intp_ot timeout);
  
  typedef intp_ot (*stream_recv_ptr)(osp_stream_t cnc, void* buf, intp_ot len);

  typedef intp_ot (*stream_send_ptr)(osp_stream_t cnc, const void* buf, intp_ot len);
  
  typedef intp_ot (*stream_close_ptr)(osp_stream_t cnc);
  
  typedef const char* (*stream_lasterr_ptr)();
  
  typedef char* (*stream_describe_ptr)(osp_stream_t cnc);

#ifdef __cplusplus
}
#endif

#endif /* __sym_stream_h__ */
