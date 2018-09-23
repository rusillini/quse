/* -*- C++ -*- */

#ifndef __time_utils_h__
#define __time_utils_h__

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Difference between tTimeValue and time_t*10000000 */
#define tmu_diffVal 116444736000000000LL
 
/* Time label type */
typedef u_int64_ot tTimeValue;

/* "Плохая" временная метка */
#define OSP_BAD_TIME ~((u_int64_ot)0)
  
typedef struct tag_TimeStruct {
  int32_ot year;
  int32_ot month, day;
  int32_ot hour, minute, second;
  int32_ot micros;
} tTimeStruct;
  
/* Parameters of the time printing functions */
  enum {
    TPR_LOCAL = 1,  /* perform conversion flag: utc->local */
    TPR_TIME = 2,   /* print time */
    TPR_DATE = 4,   /* print date */
    TPR_BDATE = 8,  /* if TPR_DATE is set, print date in yy/mm/dd format */
    TPR_MSEC = 16,  /* if TPR_TIME is set, print milliseconds */
    TPR_FNAME = 32  /* create temporary file name based on time */
  };
#define TPR_DEFAULT (TPR_TIME|TPR_DATE|TPR_BDATE)
  
  /*------------------------------------------------------------*/
  /* Get time in UI64 format */
  tTimeValue getCurrentTime(void);
  
  /* Convert time to structure and vice versa */
  void timeToStruct(const tTimeValue* tv, tTimeStruct* sv, int local);
  
  void structToTime(tTimeValue* tv, const tTimeStruct* sv, int local);
  
#ifdef __cplusplus
}
#endif

#endif /* __osp_timeUtils_h__ */
