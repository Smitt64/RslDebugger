/************************************************************************
 *  BTRVPACK.H                                                          *
 *                                                                      *
 *  Определения поддержки упаковки/разупаковки                          *
 *                                                                      *
 *  Copyright (c) 1993 JV R-Style                                       *
 *                                                                      *
 *  VERSION INFO :  $Archive: /RS-Tools/Old_tools/BEDIT/H/btrvpack.h $
 *                  $Revision: 2 $
 *                  $Date: 15.10.98 18:36 $
 *                  $Author: Markovitch $
 *                  $Modtime: 14.10.98 23:44 $
 *                                                                      *
 *  History:                                                            *
 *  ---------------------------                                         *
 *  29.10.93    ED      Created                                         *
 ************************************************************************/

#ifndef __BTRVPACK_H
#define __BTRVPACK_H

#ifndef _BTRVPACKH_
#define _BTRVPACKH_

    #include "mypack.h"
    #include "btrv.h"
    #include "bexpdef.h"

    // -----------------------------------------------------------------------------

    #ifdef __cplusplus
    extern "C" {
    #endif

    _BTEXP int RecordPackUnpk(BTRVFILE *bf, int iFixLen, char *pWorkBuffer, int iWorkLen, int iPack);
    _BTEXP int BRecordPackUnpk(BTRVFILE *bf, int iFixLen, int iPack);

    #ifdef __cplusplus
    }
    #endif

    // -----------------------------------------------------------------------------

    #define  BRecordPack(bf, iFixLen)  BRecordPackUnpk(bf, iFixLen, 1);
    #define  BRecordUnpk(bf, iFixLen)  BRecordPackUnpk(bf, iFixLen, 0);

    #define  RecordPack(bf, pw)        RecordPackUnpk(bf, _MESSAGE_FIXLEN, pw, bf->MaxRecLen, 1);
    #define  RecordUnpk(bf, pw)        RecordPackUnpk(bf, _MESSAGE_FIXLEN, pw, bf->MaxRecLen, 0);

#endif

#endif // __BTRVPACK_H

/* <-- EOF --> */