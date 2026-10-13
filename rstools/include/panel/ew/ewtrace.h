/*************************************************************************
   File           : ewtrace.h

   Description    : Трассировка

   Programmed by  : Alexey Lebedev

   Created        : 09 Oct 2001
   Last Modified  : 
   Version        : 

   Copyright (C) 2001 by R-Style Software Lab. All Rights Reserved.

*************************************************************************/

#ifndef __EWTRACE_H
#define __EWTRACE_H

#if defined(RS_TRACE) && defined(__cplusplus)
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>

    #include <iomanip>
    #include <cstring>
    #include <fstream>
    #include <strstream>

    #include "platform.h"
    #include "rscdefs.h"

    class EWTrace
        {
         public:
           static void open_trace(const char *name);
           static void close_trace();
           static void save_trace(bool flag);

           enum { Trace = 0, Message, Error };

           static const char *newline;

           static bool enabled(int level)
           {
            return _enabled[level];
           }

           static RSL_STD::string toOEM(const char *s);
           static RSL_STD::string mergeStr(const char *arr[], int n, const char *delim);
           static RSL_STD::string replaceStr(const char *s, const char *from, const char *to);
           static RSL_STD::string errorText(unsigned int errCode);

           static void print(int level, const char *);

           static void print(int level, RSL_STD::ostrstream &stream)
           {
            print(level, stream.str());

            stream.rdbuf()->freeze(false);
           }

           class format
               {
                public:
                  format(const char *fmt, ...)
                  {
                   va_list  argptr;

                   va_start(argptr, fmt);

                   if(vsprintf(_str, fmt, argptr) == EOF)
                     _str[0] = 0;

                   va_end(argptr);
                  }

                private:
                   char _str[4096];

                friend RSL_STD::ostream &operator << (RSL_STD::ostream &, const format &);
               };

         private:
           static bool  _enabled[3];
           static char  _fname[_MAX_PATH];
           static bool  _saveFlag;
           static int   _opened;

           static RSL_STD::ofstream *_pout;
        };

    inline RSL_STD::ostream &operator << (RSL_STD::ostream &os, const EWTrace::format &v)
    {
     return os << v._str;
    }

    #define  EWTRACE_OPEN(name)    EWTrace::open_trace(name)
    #define  EWTRACE_CLOSE()       EWTrace::close_trace()
    #define  EWTRACE_SAVE(flag)    EWTrace::save_trace(flag)

    #define  EWTRACE_STR(s)        '\'' << ((s) ? (s) : "") << '\''
    #define  EWTRACE_STRT(s)        '\'' << ((s) ? (s) : TEXT("")) << '\''
    #define  EWTRACE_STR2OEM(s)    '\'' << EWTrace::toOEM((s)).c_str() << '\''

    #define  EWTRACE_WNDPROC(hwnd, msg, wparam, lparam)  "hwnd=" << hwnd << ", msg=" << msg <<              \
                                                         RSL_STD::hex << RSL_STD::setfill((char)'0') <<     \
                                                         " (0x" << RSL_STD::setw(4) << wparam <<            \
                                                         ", 0x" << RSL_STD::setw(8) << lparam << ")"

    #define  EWTRACE_WNDPROCRET(hwnd, msg, ret)          "hwnd=" << hwnd << ", msg=" << msg <<              \
                                                         " returned " << ret

    #define  EWTRACE_SENDMSG(hwnd, msg, wparam, lparam)  "send message to "                                 \
                                                         "hwnd=" << hwnd << ", msg=" << msg <<              \
                                                         RSL_STD::hex << RSL_STD::setfill((char)'0') <<     \
                                                         " (0x" << RSL_STD::setw(4) << wparam <<            \
                                                         ", 0x" << RSL_STD::setw(8) << lparam << ")"

    #define  EWTRACE_POSTMSG(hwnd, msg, wparam, lparam)  "post message to "                                 \
                                                         "hwnd=" << hwnd << ", msg=" << msg <<              \
                                                         RSL_STD::hex << RSL_STD::setfill((char)'0') <<     \
                                                         " (0x" << RSL_STD::setw(4) << wparam <<            \
                                                         ", 0x" << RSL_STD::setw(8) << lparam << ")"

    #define  EWTRACE(m)                                                           \
             {                                                                    \
                if (EWTrace::enabled (EWTrace::Trace))                            \
                {                                                                 \
                   RSL_STD::ostrstream stream_;                                   \
                   stream_ << m << RSL_STD::ends;                                 \
                   EWTrace::print(EWTrace::Trace, stream_);                       \
                }                                                                 \
             }

    #define  EWMESG(m)                                                            \
             {                                                                    \
                if (EWTrace::enabled (EWTrace::Message))                          \
                {                                                                 \
                   RSL_STD::ostrstream stream_;                                   \
                   stream_ << m << RSL_STD::ends;                                 \
                   EWTrace::print(EWTrace::Message, stream_);                     \
                }                                                                 \
             }

    #define  EWERR(m)                                                             \
             {                                                                    \
                if (EWTrace::enabled (EWTrace::Error))                            \
                {                                                                 \
                   RSL_STD::ostrstream stream_;                                   \
                   stream_ << m << RSL_STD::ends;                                 \
                   EWTrace::print(EWTrace::Error, stream_);                       \
                }                                                                 \
             }

    #define  EWSYSERR(m)                                                          \
             {                                                                    \
                if (EWTrace::enabled (EWTrace::Error))                            \
                {                                                                 \
                   unsigned int code = ::GetLastError();                          \
                   RSL_STD::ostrstream stream_;                                   \
                   stream_ << m << RSL_STD::endl                                  \
                            << "ERROR! " << code << " - "                         \
                            << EWTrace::errorText(code).c_str() << RSL_STD::ends; \
                   EWTrace::print(EWTrace::Error, stream_);                       \
                }                                                                 \
             }
#else
    #define  EWTRACE_OPEN(name)
    #define  EWTRACE_CLOSE()
    #define  EWTRACE_SAVE(flag)

    #define  EWTRACE_STR(s)
    #define  EWTRACE_STR2OEM(s)

    #define  EWTRACE_WNDPROC(hwnd, msg, wparam, lparam)
    #define  EWTRACE_WNDPROCRET(hwnd, msg, ret)
    #define  EWTRACE_SENDMSG(hwnd, msg, wparam, lparam)
    #define  EWTRACE_POSTMSG(hwnd, msg, wparam, lparam)

    #define  EWTRACE(m)
    #define  EWMESG(m)
    #define  EWERR(m)
    #define  EWSYSERR(m)
#endif

#endif //__EWTRACE_H

/* <-- EOF --> */