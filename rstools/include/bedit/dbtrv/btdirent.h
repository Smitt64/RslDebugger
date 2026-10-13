/*******************************************************************************
 FILE         :   BTDIRENT.H

 COPYRIGHT    :   R-Style Software Lab, 1999

 DESCRIPTION  :   DBTriever/400 directory functions, analogous to
                  POSIX.

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   29 Jan 1999

 VERSION INFO :   $Archive: /RSTool400/DBTrv400/Include/btdirent.h $
                  $Revision: 3 $
                  $Date: 30.04.99 14:14 $
                  $Author: Markovitch $
                  $Modtime: 27.04.99 15:56 $
*******************************************************************************/

#ifndef __BTDIRENT_H
#define __BTDIRENT_H

#include <sys/stat.h>

// -----------------------------------------------
#define  DB4_IS_DIRECTORY(ent)  (!((ent).datasize))

/*
   DBTriever Directory Stream.  This is an incomplete type.
*/
typedef struct _BT_DIR  BTDIR;

typedef struct stat     btstat;

/*******************************************************************************
                  struct db4_dirent

 DBTriever Directory Entry.
 Invariant to alignment (packing level)
*******************************************************************************/
typedef struct db4_dirent
      {
       unsigned long     attrib;       /* BTrieve file or key flags */
       unsigned short    numfields;    /* Number of fields or segments */
       unsigned short    datasize;     /* Record or key size */
       unsigned short    numndxes;     /* Num of keys (0 if KEYNUM >0) */

       char              btrvname[24]; /* BTrieve filename (DOS 16.6) */
       char              reserved[11]; /* Don't touch! */
       char              padding[3];
      } db4_dirent;


#ifdef __cplusplus
extern "C" {
#endif

/*
   db4opendir()   -  Open DBtriever Directory Stream

   Opens DBTriever directory so that it can be read with the db4readdir() function
   Parameters:
      dirname  -  name of the directory to open.
                  Can be in the form of single library name (such as "QRSBANK") or
                  /QSYS.LIB name (such as "/QSYS.LIB/RSBANK.LIB")

   Return value:
      DBtriever directory handle that can be used in subsequent calls ro
      db4readdir(), db4rewinddir(), db4closedir()

      NULL if db4reddir reaches the end of the directory stream.
*/
BTDIR *db4opendir(const char *dirname);

/*
   db4readdir()   -  Read Next DBtriever Directory Entry

   Parameters:
      dir   -  directory handle opened by db4opendir.

   Return value:
      Pointer to db4_dirent structure describing next directory entry,
      if call was successfull.

      NULL if db4reddir reaches the end of the directory stream.
*/
db4_dirent *db4readdir(BTDIR *dir);

/*
   db4rewinddir() -  Rewind DBtriever Directory Stream

   Resets position of the DBTriever directory stream to the beginning.

   Parameters:
      dir   -  directory handle opened by db4opendir.
*/
void db4rewinddir(BTDIR *dir);

/*
   db4closedir() -  Close DBtriever Directory Stream

   Closes DBTriever directory stream previously opened by db4open()
   Don't forget to close directory, because of resource freeing.

   Parameters:
      dir   -  directory handle opened by db4opendir.
*/
void db4closedir(BTDIR *dir);

/*
   db4access()    -  Check DBTriever File for Existence

   Determines whether specified DBTriever file exist and whether you can get access
   to it in given mode.

   Parameters:
      btpath   -  DBTriever file path (in any form - native or /QSYS.LIB)
      mode     -  access mode. Possible values are:
                  06 -  check for permission to read from and write to the file;
                  04 -  check for permission to read from the file;
                  02 -  check for permission to write to the file;
                  00 -  check only for the existence of the file

   Return value:
      0  if you can get access to the file in the specified mode
      -1 if file does not exist or cannot be accessed in the specified mode
*/
int db4access(const char *btpath, int mode);

/*
   db4stat()      -

   Parameters:
      btname   -  DBtreiver file name
      buffer   -  buffer for returned status structure
*/
int db4stat(const char *btname, btstat *buffer);

/*
   db4filesize()  - получить размер всех, физическиз и логических, файлов,
                    связанных с Btreiev-файлом
   Parameters:
      btname   -  DBtreiver file name
*/
unsigned long db4filesize(const char *btname);

/*
   db4filename()  - получить имя файла, эмулирующего определенный ключ
                    Btreiev-файла
   Parameters:
      btname   -  DBtreiver file name
      keynum   -  номер ключа (-1 для физического файла)
      buffer   -  буффер для имени файла DB/2
*/
int db4filename(const char *btname, int keynum, char *buffer);

#ifdef __cplusplus
}
#endif

#endif // __BTDIRENT_H

/* <-- EOF --> */