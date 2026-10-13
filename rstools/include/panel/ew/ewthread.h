//-*--------------------------------------------------------------------------*-
//
// File Name   : ewthread.h
//
// Copyright (c) 1991 - 2006 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// September 26,2007  Sergei Kubrin (K78) - Create file
//-*--------------------------------------------------------------------------*-

#ifndef __EWTHREAD_H
#define __EWTHREAD_H

class EwThread
    {
     public:
       TSmartObj  elem;

     protected:
       DWORD       tId;
       uintptr_t   th;
       HANDLE      readyEvent;
       bool        autoDelete;

       void signalStarted()
       {
        SetEvent (readyEvent);
       }

       virtual bool waitForStart()
       {
        return WaitForSingleObject(readyEvent, INFINITE) == WAIT_OBJECT_0;
       }

       virtual bool waitForComplete(DWORD tmOut)
       {
        return WaitForSingleObject((HANDLE)th, tmOut) == WAIT_OBJECT_0;
       }

       virtual unsigned run() = 0;

       virtual bool init()
       {
        signalStarted();

        return true;
       }

       virtual void done()
       {
       }


       static unsigned __stdcall EntryPoint(void *dt)
       {
        EwThread  *obj = (EwThread *)dt;


        obj->tId = GetCurrentThreadId();

        unsigned  stat = obj->init ();


        if(stat)
          stat = obj->run();

        obj->done();

        if(obj->autoDelete)
          delete obj;

        obj->tId = 0;

        return stat;
       }

       static void AddThreadInList(EwThread *ptr);

       EwThread() : tId(0), th(0), autoDelete(false)
       {
        readyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
       }

       virtual ~EwThread()
       {
        if(th)
          CloseHandle((HANDLE)th);

        if(readyEvent)
          CloseHandle(readyEvent);
       }

       bool start()
       {
        th = _beginthreadex(NULL, 0, EntryPoint, this, 0, NULL);

        if(th)
         {
          waitForStart();

          CloseHandle(readyEvent);

          readyEvent = NULL;

          AddThreadInList(this);

          return true;
         }

        return false;
       }

     public:

       static void RemAllThreads();

       void stop()
       {
        if(tId)
          PostThreadMessage(tId, WM_QUIT, 0, 0);
       }

       bool wait(DWORD tmOut)
       {
        if(!th)
           return true;

        stop();

        if(waitForComplete(tmOut))
          return true;

        return false;
       }
    };

#endif // __EWTHREAD_H

/* <--EOF --> */