/*******************************************************************************
 FILE         :   DB4DIRUTIL.H

 COPYRIGHT    :   R-Style Software Lab, 1999

 DESCRIPTION  :   Copy/clone/rename/move BTrieve files. 

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   1 Jul 1999

 VERSION INFO :   $Archive: /RSTool400/DBTrv400/Include/db4dirutil.h $
                  $Revision: 2 $
                  $Date: 2.07.99 22:23 $
                  $Author: Markovitch $
                  $Modtime: 2.07.99 21:16 $
*******************************************************************************/

#ifndef __DB4DIRUTIL_H
#define __DB4DIRUTIL_H

/*******************************************************************************
 Utility options
*******************************************************************************/
#define  BTOPT_CPYDATA      (0x0001) /* Copy file data on clone */
#define  BTOPT_REPLACE      (0x0002) /* Replace target file if exist */
#define  BTOPT_DELMOVE      (0x0004) /* Move deleted data files in QRPLOBJ */
#define  BTOPT_NDELFILES    (0x0008) /* Don't delete data files deleting Btrieve file description */

#ifdef __cplusplus
extern "C" {
#endif

   void db4clone(const char *from, const char *to, unsigned options);

   void db4delete(const char *fname, int nodelfile);
   
   void db4renmove(const char *from, const char *to, unsigned options);
      
#ifdef __cplusplus
}
#endif

#endif /* __DB4DIRUTIL_H */

/* <-- EOF --> */