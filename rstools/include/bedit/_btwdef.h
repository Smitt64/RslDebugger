/*******************************************************************************
 FILE         :   _BTWDEF.H

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   Some work declarations/definitions for BF library.
                  For internal use only.

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   14 Sep 1998

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/bedit/h/_btwdef.h $
                  $Revision: 2 $
                  $Date: 6.04.00 17:19 $
                  $Author: Kubrin $
                  $Modtime: 27.03.00 18:35 $
*******************************************************************************/

#ifndef ___BTWDEF_H
#define ___BTWDEF_H

#define BFILE_POS_BLK_H(fl) (&((fl)->internal.posblk))

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
int bfIPutChunk(
                BTRVFILE *bf,
                unsigned long bias,  // May be BT_NEXT_BIAS or BT_APPEND_BIAS
                void         *buff,  // Buffer to put chunk data. If NULL use data
                                     // from TBlobControlData.
                unsigned long offs,  // Offset of chunk in record
                unsigned  long len   // Lenght of chunk. if 0 than use data
                                     // from TBlobControlData.
               );

#ifdef __cplusplus
}
#endif

#endif // ___BTWDEF_H

/* <-- EOF --> */