/* -*- C++ -*- */

// $Header: /mounts/times2/disks/0/cvsroot/quse/utils/src/tmutil.c,v 1.1.1.1 2009/03/19 19:14:06 akotov2 Exp $

#include <tmutil.h>
#include <time.h>
#include <sys/time.h>

tTimeValue getCurrentTime()
{
  struct timeval t;
  tTimeValue tv = 0;
  
  if ( gettimeofday(&t, NULL)!=0 )
    return 0;
  
  tv = tmu_diffVal;
  tv += ((tTimeValue)t.tv_sec) * 10000000;
  tv += t.tv_usec * 10;
  
  return tv;
}

void timeToStruct(const tTimeValue* tv, tTimeStruct* sv, int local)
{
  time_t tmpTime;
  struct tm t;
  
  tmpTime = (*tv - tmu_diffVal) / 10000000;
  if ( local )
    localtime_r(&tmpTime, &t);
  else
    gmtime_r(&tmpTime, &t);
  sv -> year = 1900 + t.tm_year;
  sv -> month = 1 + t.tm_mon;
  sv -> day = t.tm_mday;
  sv -> hour = t.tm_hour;
  sv -> minute = t.tm_min;
  sv -> second = t.tm_sec;
  sv -> micros = (int)((*tv / 1000) % 10000);
}

void structToTime(tTimeValue* tv, const tTimeStruct* sv, int local)
{
  time_t tmpTime;
  struct tm t;
  int Xcentury, Xyear, Xmonth, Xday;
  
  if ( sv->year < 1901 )
    local = 0;
  
  if ( local ) {
    t.tm_year = sv -> year - 1900;
    t.tm_mon = sv -> month - 1;
    t.tm_mday = sv -> day;
    t.tm_hour = sv -> hour;
    t.tm_min = sv -> minute;
    t.tm_sec = sv -> second;
    t.tm_isdst = -1;
    tmpTime = mktime(&t);
    *tv = tmu_diffVal;
    *tv += tmpTime * (tTimeValue)10000000;
  } else {
    
    if ( sv->year>1900 ) {
      Xcentury = sv->year / 100;
      Xyear    = sv->year % 100;
      Xmonth   = sv->month;
      Xday     = sv->day;
      
      if (Xmonth > 2)
        Xmonth -= 3;
      else {
        Xmonth += 9;
        if (Xyear)
          Xyear--;
        else {
          Xyear = 99;
          Xcentury--;
        }
      }
      *tv = (((tTimeValue)146097) * Xcentury) / 4 + (1461 * Xyear) / 4 +
        (153 * Xmonth + 2) / 5 + Xday + 1721119;
      
      *tv -= 2305814;
      *tv *= 24 * 60 * 60;
    } else
      *tv = 0;
    *tv += sv->hour*3600 + sv->minute*60 + sv->second;
    *tv *= 10000; *tv += sv->micros; *tv *= 1000;
  }
}
