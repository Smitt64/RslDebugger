/*******************************************************************************
 FILE         :   NWBTRV.CC

 COPYRIGHT    :   R-Style Software Lab, 1999

 DESCRIPTION  :   Miscellaneous Btrieve API support

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   19 Jul 1999

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/bedit/h/nwbtrv.cc $
                  $Revision: 4 $
                  $Date: 22.09.00 17:43 $
                  $Author: Lebedev $
                  $Modtime: 22.09.00 17:27 $
*******************************************************************************/

#ifndef __NWBTRV_CC
#define __NWBTRV_CC

#ifndef NULL
#include <stdio.h>
#endif

#ifdef __BORLANDC__
#pragma warn -inl
#endif

// -----------------------------------------------------------------------------
_rs_inline_ const KeySpecs *keyspec_end(const KeySpecs *begin)
{
 if(begin)
   while (begin++->keyflag & (int)Ksegm);

 return begin;
}
   
// -----------------------------------------------------------------------------
_rs_inline_ const KeySpecs *keyseg_range(const KeySpecs *begin, int keynum, const KeySpecs **end = NULL)
{
 while(keynum-- > 0 && (begin = keyspec_end(begin)) != NULL);

 if(end)
   *end = keyspec_end(begin);

 return begin;
}
   
#endif /* __NWBTRV_CC */

/* <-- EOF --> */