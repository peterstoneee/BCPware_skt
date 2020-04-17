/************************************ log.inl ******************************************
*    A Header that define macros for printing log.                                     *
*    These functions are used to record log, logs are useful when you need to debug.   *
*                                                                                      *
*    DO NOT MULTI INCLUDE THIS HEADER                                                  *
*    (Only include this header in cpp file will avoid this situation)                  *
*                                                                                      *
*    You must define these macros before include this header:                          *
*    LOGNAME - A C-string specify the log file name                                    *
*    LOGLEVEL - A number determine how detailed log should showed                      *
*               3: show all                                                            *
*               2: show level2-print, warn, critical, and function entering and        *
*                  exit                                                                *
*               1: only show critical and function entering and return                 *
*               0: show nothing                                                        *
*    LOG_STDOUT - (optional) Define it if you want show log on stdout                  *
*    /////FOR EXAMPLE//////////////                                                    *
*    #define LOGNAME "my.log"                                                          *
*    #define LOG_STDOUT                                                                *
*    #define LOGLEVEL 3                                                                *
*    //////////////////////////////                                                    *
*                                                                                      *
*    Usage:                                                                            *
*    1. - Put ENTERFUNC at the very beginning of function body.                        *
*       - Use LEAVEFUNC instead 'return' to return the function, if a value is needed  *
*         to return, use LEAVEFUNC2(retval) to return.                                 *
*       - A void-return-type function is also needed to put a LEAVEFUNC at the most    *
*         end of function body.                                                        *
*    2. Use log_println() , log_println2(...) , log_warnln() , log_criticalln() like   *
*       as printf()                                                                    *
*       - log_println()    : For normal message                                        *
*       - log_println2()   : Level2-print, used by more important normal message       *
*       - log_warnln()     : For warning message                                       *
*       - log_criticalln() : For critical error, I used it when the code detect a      *
*                            logic error.                                              *
****************************************************************************************/

#include <stdio.h>
#include <time.h>
#include "define_some_parameter.h"
//#include <string.h>

#if(LOGLEVEL>0)

  #define LOG_NEWLINE "\n       | "

  static FILE *_log_fptr = fopen(LOGNAME, "wb");
  static int _log_funclv = 0;

  #ifdef LOG_STDOUT
    #define ENTERFUNC_STDOUT \
    fprintf(stdout, ">>>% 2d>>> Enter: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
    fflush(stdout); 
    #define LEAVEFUNC_STDOUT \
    fprintf(stdout, "<<<% 2d<<< Leave: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
    fflush(stdout);
    #define PRINTLN_STDOUT(printstr, va_args) \
    fprintf(stdout, printstr, _log_funclv); \
    fprintf(stdout, va_args); \
    fprintf(stdout, " (%s:%d)\n", __FILE__, __LINE__); \
    fflush(stdout);
  #else
    #define ENTERFUNC_STDOUT
    #define LEAVEFUNC_STDOUT
    #define PRINTLN_STDOUT(printstr, va_args)
  #endif

  #define ENTERFUNC { \
  ++_log_funclv; \
  fprintf(_log_fptr, ">>>% 2d>>> Enter: %s  Clock: %d\n", _log_funclv, __FUNCTION__, clock()); \
  fflush(_log_fptr); \
  ENTERFUNC_STDOUT \
  }

  #define LEAVEFUNC { \
  fprintf(_log_fptr, "<<<% 2d<<< Leave: %s  Clock: %d  (%s:%d)\n", _log_funclv, __FUNCTION__, clock(), __FILE__, __LINE__); \
  fflush(_log_fptr); \
  LEAVEFUNC_STDOUT \
  --_log_funclv; \
  return; \
  }
  #define LEAVEFUNC2(retval) { \
  fprintf(_log_fptr, "<<<% 2d<<< Leave: %s Return:%s Clock: %d  (%s:%d)\n", _log_funclv, __FUNCTION__, ""#retval, clock(), __FILE__, __LINE__); \
  fflush(_log_fptr); \
  LEAVEFUNC_STDOUT \
  --_log_funclv; \
  return (retval); \
  }

  #if (LOGLEVEL>=3)
    #define log_println(...) { \
    fprintf(_log_fptr, "   % 2d  | ", _log_funclv); \
    fprintf(_log_fptr, __VA_ARGS__); \
    fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
    fflush(_log_fptr); \
    PRINTLN_STDOUT("   % 2d  | ", __VA_ARGS__) \
    }
  #else
    #define log_println(...)
  #endif

  #if (LOGLEVEL>=2)
    #define log_println2(...) { \
    fprintf(_log_fptr, "   % 2d  | ", _log_funclv); \
    fprintf(_log_fptr, __VA_ARGS__); \
    fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
    fflush(_log_fptr); \
    PRINTLN_STDOUT("   % 2d  | ", __VA_ARGS__) \
    }
    
    #define log_warnln(...) { \
    fprintf(_log_fptr, "!  % 2d  | ", _log_funclv); \
    fprintf(_log_fptr, __VA_ARGS__); \
    fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
    fflush(_log_fptr); \
    PRINTLN_STDOUT("!  % 2d  | ", __VA_ARGS__) \
    }
  #else
    #define log_println2(...)
    #define log_warnln(...)
  #endif

  #if (LOGLEVEL>=1)
    #define log_criticalln(...) { \
    fprintf(_log_fptr, "x  % 2d  | ", _log_funclv); \
    fprintf(_log_fptr, __VA_ARGS__); \
    fprintf(_log_fptr, " (%s:%d)\n", __FILE__, __LINE__); \
    fflush(_log_fptr); \
    PRINTLN_STDOUT("x  % 2d  | ", __VA_ARGS__) \
    }
  #else
    #define log_criticalln(...)
  #endif


#else
  #define ENTERFUNC
  #define LEAVEFUNC  return;
  #define LEAVEFUNC2(retval) { return (retval); }
  #define log_println(...) {}
  #define log_println2(...)
  #define log_warnln(...) {}
  #define log_criticalln(...) {}
#endif

//******************************************************
//May 12 2017-Timmy - add test log
//filename format: {Test_YYYY_MMDD.log}, update daily
static char s_testlogfilename[25];
static FILE *pFileTestlog = NULL;
//static FILE *pFileTestlog = fopen("Test_2017_0512.log", "a");
static char s_templog[255]; //share for outside
//static char s_date[9];
static char s_time[9];
static time_t timer;
static struct tm* tm_info;  //use in this log macro inner only


static char s_InnerTempLog[255]; 
//2017.6.15 - Timmy - set print on screen for CIMation Comm
#define pr printf
//FILE *pFileTestlog = fopen(s_testlogfilename, "a"); \

#define testlog(retval) {\
  _strtime(s_time);\
  time(&timer);tm_info = localtime(&timer);\
  strftime(s_testlogfilename, 26, "test_%Y_%m%d.log", tm_info);\
  sprintf(s_InnerTempLog, "%s, %s\n", s_time, retval);\
  pr(s_InnerTempLog);\
  FILE *pFileTestlog = fopen(getLogFileName().toLatin1().data(), "a"); \
  fprintf(pFileTestlog, s_InnerTempLog);\
  fflush(pFileTestlog);\
  fclose(pFileTestlog);\
}


#define USBSendLog(retval) {\
  _strtime(s_time);\
  time(&timer);tm_info = localtime(&timer);\
  strftime(s_testlogfilename, 26, "USBSendLog_%Y_%m%d.log", tm_info);\
  sprintf(s_InnerTempLog, "%s, %s\n", s_time, retval);\
  pr(s_InnerTempLog);\
  FILE *pFileTestlog = fopen(s_testlogfilename, "a"); \
  fprintf(pFileTestlog, s_InnerTempLog);\
  fflush(pFileTestlog);\
  fclose(pFileTestlog);\
}

//strcpy(s_testlogfilename, "Test_2017_0512.log"); \
//_strdate(s_date);
/*
sprintf(s_templog, "%s: %s\n", s_time, retval); \
pr(s_templog); \

fprintf(pFileTestlog, "%s: %s\n",s_time, retval);\
//printf("%s: %s\n",s_time, retval);\
#define testlog2(...) {\
   _strtime(s_time); fprintf(pFileTestlog, "%s: %s\n",s_time);\
   fprintf(pFileTestlog, __VA_ARGS__);\
   fflush(pFileTestlog);\
}
*/

//Timmy put backup statement here
//_strdate(s_date);
//fprintf(pFileTestlog, "%s %s: %s\n",s_date, s_time, retval);\
// end of backup

//May 12 - Timmy - test log fun 
//
/*
void testlogstr(_In_z_ _Printf_format_string_ const char * _Format, ...)
{
	va_list args;
	va_start(args, _Format);

	_strdate(s_date); _strtime(s_time);
	fprintf(pFileTestlog, "%s %s", s_date, s_time);
	fprintf(pFileTestlog, _Format, va_arg(args, int), va_arg(args, int));
	//va_arg(args, char*);
	va_end(args);
	//fflush(pFileTestlog);	
}
*/
//******************************************************