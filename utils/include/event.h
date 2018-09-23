/* -*- C++ -*- */

#ifndef __osp_event_h__
#define __osp_event_h__

#include <config.h>

#define OSP_EVENT_BAD NULL

#ifdef __cplusplus
extern "C" {
#endif
  
  typedef void* osp_event_t;
  
  osp_event_t osp_event_create();
  int osp_event_destroy(osp_event_t event);
  int osp_event_signal(osp_event_t event);
  int osp_event_reset(osp_event_t event);
  int osp_event_wait(osp_event_t event, int timeout, int reset);
  
#ifdef __cplusplus
}
#endif

#endif /* __osp_event_h__ */
