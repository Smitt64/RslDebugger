/*******************************************************************************
FILE         :   rsfmt.h
COPYRIGHT    :   R-Style, 2000 - 2002
DESCRIPTION  :   Описание структур данных RS SQL словаря
PROGRAMMED BY:   Nechaev Pavel
CREATION DATE:   04/10/2002
LAST UPDATE  :   04/10/2002
*******************************************************************************/

#ifndef __RSFMT_H__
#define __RSFMT_H__

/******************************************************************************/
/******************************************************************************/

#define  RSFMT_NAME_MAXSIZE         256
#define  RSFMT_COMMENT_MAXSIZE      129
#define  RSFMT_SKIP_SEG_IN_INDEX    0x8000
#define  RSFMT_LOCAL_SEG_INDEX      0x4000

#define  RSFMTKSEG_SETEND(_p_kseg)  (_p_kseg)->fld = -1
#define  RSFMTKSEG_END(_p_kseg)     ((_p_kseg)->fld == -1)

#define  RSD_AUTOINCFLD_NAME        "t_autoinc"

/******************************************************************************/
// -----------------------------------------------
typedef enum tagRSDICT_FLAGS
      {
       RSDF_PK_AUTOINC       = 0x02, // autoincrement PK
       RSDF_GLOBAL_TEMPORARY = 0x10, // GLOBAL TEMPORARY Table
       RSDF_STRUCTURE        = 0x20  // structure
      } RSDICT_FLAGS;

// -----------------------------------------------
typedef struct tagRSDICT_FMTFLD
      {
       long            type;         // BT type: enum TFVT { FT_... }
       long            length;       // if obType == LPSTR 
       unsigned long   ordinal;      // the number of parameters/columns
       long            valueOff;     // Value offset
       char            name[RSFMT_NAME_MAXSIZE];
       char            comment[RSFMT_COMMENT_MAXSIZE];
       unsigned short  outLen;       // Length for output
       short           decPoint;
       unsigned char   hidden;
      } RSDICT_FMTFLD;

// -----------------------------------------------
typedef struct tagRSDICT_FMTKSEG
      {
       long            fld;
       short           flags;        // BT flags
       long            type;         // BT type
       unsigned char   nullval;
       char            comment[RSFMT_COMMENT_MAXSIZE];
      } RSDICT_FMTKSEG;

// -----------------------------------------------
typedef struct tagRSDICT_FMT
      {
       long              tid;            // table id

       char              owner[9];
       char              comment[RSFMT_COMMENT_MAXSIZE];

       long              cache_size;

       // Blob
       short             blob_type;      // enum BT_BLOB_...
       long              blob_maxvarlen;

       // Fields
       unsigned long     nfields;
       RSDICT_FMTFLD    *fields;
       
       // Keys
       short             pk_index;    // The number of Primary Key or -1
       unsigned long     nkeys;
       RSDICT_FMTKSEG  **keys;
       long              flags;
      } RSDICT_FMT;

/******************************************************************************/
/******************************************************************************/

#endif   // __RSFMT_H__

/* <-- EOF --> */