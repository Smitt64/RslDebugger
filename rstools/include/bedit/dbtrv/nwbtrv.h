/*******************************************************************************
 FILE         :   NWBTRV.H

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   nwbtrv.h for AS400. Base C definitions for BTrieve emulator
                  on AS400

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   20 Feb 1998

 VERSION INFO :   $Archive: /RSTool400/DBTrv400/Include/nwbtrv.h $
                  $Revision: 41 $
                  $Date: 5.08.99 20:29 $
                  $Author: Markovitch $
                  $Modtime: 5.08.99 18:15 $
******************************************************************************/

#ifndef __NWBTRV_H
#define __NWBTRV_H

#include <platform.h>
#include <bdate.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
typedef enum Btrieve_Operations
      {
       Bunimplemented = -2, /* Unimplemented operation */

       Bnop     = -1, /* Incorrect operation */

       Bopen    = 0,  /* Makes a file available for access */

       Bclose   = 1,  /* Releases a file from availability */
                      /* Only pos block needed */

       Binsert  = 2,  /* Inserts a new record into a file */

       Bupdate  = 3,  /* Updates the current record */

       Bdelete  = 4,  /* Removes the current record from the file */
                      /* Record length & key number neded */

       BgetE    = 5,  /* Gets the record whose key value */
                      /* matches the reuested key value */

       BgetN    = 6,  /* Gets the record following the current */
                      /* record in the index path */

       BgetP    = 7,  /* Gets the record preceding the current */
                      /* record in the index path */

       BgetGT   = 8,  /* Gets the record whose key value */
                      /* is greater then the reuested key value */

       BgetGE   = 9,  /* Gets the record whose key value is equal to */
                      /* or greater then the reuested key value */

       BgetLT   = 10, /* Gets the record whose key value */
                      /* is less then the reuested key value */

       BgetLE   = 11, /* Gets the record whose key value is equal to */
                      /* or less then the reuested key value */

       BgetF    = 12, /* Gets the first record in the requested access path */

       BgetL    = 13, /* Gets the last record in the requested access path */

       Bcreate  = 14, /* Creates a Btrieve file. File name in the key buffer */

       Bstat    = 15, /* Returns file and index characteristics and number of records */

       Bextend  = 16, /* Extends a file across two disk volumes. File name in the key buffer */

       Bsetdir  = 17, /* Changes the current directory to string value in key buffer */

       Bgetdir  = 18, /* Returns the current directory in key buffer. Key number 1=A,2=B,0=default drive */

       BBT      = 19, /* Begin transaction. No parametrs */

       BET      = 20, /* End transaction. No parametrs */

       BAT      = 21, /* Abort transaction. No parametrs */

       Bgetpos  = 22, /* Gets the position of current record */

       BgetD    = 23, /* Gets the record at a specified position */

       BstepN   = 24, /* Gets the record from the physical location */
                      /* following the current record */

       Bstop    = 25, /* Terminates the memory resident Record Manager programs at a workstation */

       Bver     = 26, /* Returns the currently loaded version of the Record Menager */

       Bunlock  = 27, /* Unlocks a record or records */

       Breset   = 28, /* Releases all resources held by a workstation */

       Bsetown  = 29, /* Assigns an owner name to a file */

       Bclearown= 30, /* Removes an owner name from a file. Only pos block needed. */

       BcreateSI= 31, /* Creates a supplemental index. KeySpecs mast be in data buffer. */

       BdropSI  = 32, /* Removes a supplemental index. Key number needed */

       BstepF   = 33, /* Returns the record in the first physical location in the file */

       BstepL   = 34, /* Returns the record in the last physical location in the file */

       BstepP   = 35, /* Returns the record from the physical */
                      /* location preceding the current record */

       BgetNX   = 36, /* Get Next Extended */

       BgetPX   = 37, /* Get Previous Extended */

       BstepNX  = 38, /* Step Next Extended */

       BstepPX  = 39, /* Step Previous Extended */

       BinsertX = 40, /* Insert Extended */

       BgetPcnt = 44, /* Get By Percentage */

       BfindPcnt= 45, /* Find Percentage */

       BupdateChunk  = 53,    /* Updates chunk */

       BgkE     = 55, /* Gets key of the record whose key value */
                      /* matches the reuested key value */

       BgkN     = 56, /* Gets key of the record following the current */
                      /* record in the index path */

       BgkP     = 57, /* Gets key of the record preceding the current */
                      /* record in the index path */

       BgkGT    = 58, /* Gets key of the record whose key value */
                      /* is greater then the reuested key value */

       BgkGE    = 59, /* Gets key of the record whose key value is */
                      /* equal to or greater then the reuested key value */

       BgkLT    = 60, /* Gets key of the record whose key value */
                      /* is less then the reuested key value */

       BgkLE    = 61, /* Gets key of the record whose key value is */
                      /* equal to or less then the reuested key value */

       BgkF     = 62, /* Gets key of the first record in the requested access path */

       BgkL     = 63, /* Gets key of the last record in the requested */

       BcreateX = 1014, /* Creates a Btrieve file using extended info (fields). */
       
       BmaxOp
      } Btrieve_Operations;


/********************************* Locks ******************************* */

typedef enum Locks
      {
       SWL           =  100,           /* Single record wait lock */
       SNL           =  200,           /* Single record nowait lock */
       MWL           =  300,           /* Multiple record wait lock */
       MNL           =  400,           /* Multiple record nowait lock */
       NOWAITMODIFY  =  500,           /* Modification ops (Upd, Ins, Del) in transaction are no-wait */
       TRNCONCURRENT = 1000,           /* Transaction is concurent V.6.x */
       TRNCONCURENT  = TRNCONCURRENT   /* For compatibility with old typo :) */
      } Locks;

/********************************* Open operation support ************** */

typedef enum OpenModes
      {
       Accelerated = -1,     /* Disable data recovery capability */
       ReadOnly    = -2,     /* Disable updating */
       Verify      = -3,     /* Dos verify mode */
       Exclusive   = -4,     /* Exclusive access mode */
       Normal      = 0       /* Normal mode */
      } OpenModes;

// -----------------------------------------------
/* Extended_Key_Type */
#define  Estring   '\x00'   /* sequence of char from left to right */
#define  Einteger  '\x01'   /* even number of bytes */
#define  Efloat    '\x02'   /* IEEE standard 4 & 8 bytes */
#define  Edate     '\x03'   /* int year, char month, char day */
#define  Etime     '\x04'   /* char hour,minute,second,hundredths */
#define  Edecimal  '\x05'   /* ANSI standard packed decimal */
#define  Emoney    '\x06'   /* same decimal */
#define  Elogical  '\x07'   /* 1 or 2 byte string */
#define  Enumeric  '\x08'   /* zoned format */
#define  Ebfloat   '\x09'   /* Microsoft basic standard */
#define  Elstring  '\x0A'   /* Pascal compatible string */
#define  Ezstring  '\x0B'   /* C compatible string */
#define  Eunbin    '\x0E'   /* unsigned integers */
#define  Eautoinc  '\x0F'   /* Autoincrement */

#define  EMONEY  Emoney

/*******************************************************************************
                           enum BtrvLimits
 Некоторые ограничение BTrieve
*******************************************************************************/
typedef enum BtrvLimits
      {
       BTMaxSegments  =  119,
       BTMaxFields    =  120,
       BTMaxKeySize   =  255,
       BTMaxAutoinc   =  3,

       BTMaxFileName  =  16,
       BTMaxFileExt   =  6,
       BTMaxFile      =  BTMaxFileName + BTMaxFileExt + 2, /* name.ext\0 (24) */
       BTMaxPath      =  BTMaxFile + 11                    /* lib/name.ext\0 */
      } BtrvLimits;


/*******************************************************************************
                     enum UnlockMode
 Способы разблокировок заблокированных записей
*******************************************************************************/
enum UnlockMode
   {
    UnlockSingle =  0,  /* Unlock single record lock */
    UnlockOne    = -1,  /* Unlock one record multiply locked */
    UnlockAll    = -2   /* Unlock all records multiply locked */
   };

/*******************************************************************************
 enum OwnerMode
 Password modes
*******************************************************************************/
enum OwnerMode
   {
    SetORW  =  0,      /* Requiers for any mode,no encryption */
    SetOW   =  1,      /* Permits read-only mode,no encryption */
    SetORWE =  2,      /* Requiers for any mode, encryption */
    SetOWE  =  3       /* Permits read-only mode, encryption */
   };

/************************************************************************/

#define  Berors  BError

/*******************************************************************************
                        enum BError

 Pseude-BTrieve and emulator return codes
*******************************************************************************/
enum BError
   {
    BEunimplemented = -1, /* Extended code - operation is not implemented yet */
    BEOK            = 0, /* By (JNS) */
    BEinoper        = 1, /* Illegal Operation Code */
    BEioerr         = 2,
    BEnoopen        = 3,
    BEnorec         = 4,
    BEdupkey        = 5,
    BEinvkey        = 6,
    BEdifkey        = 7,
    BEinpos         = 8,
    BEeof           = 9,
    BEmodkey        = 10,
    BEfilenm        = 11,
    BEnofile        = 12,
    BEexfile        = 13,
    BEclose         = 17,
    BEfulld         = 18,
    BEKrnInactive   = 20,
    BEldata         = 22,
    BEnobfile       = 30,
    BEnumrec        = 43,
    BEmaxop         = 47,
    BEowner         = 50,
    BEefile         = 59,
    BEdes           = 62,
    BEdeadLock      = 78,
    BEconflict      = 80,
    BElostpos       = 82,
    BErecloc        = 84,
    BEfileloc       = 85,
    BEtableful      = 86,
    BEchunkOffs     = 103,
    BEmaxerr, 

    /* Emulator errors */
    BEillopparm     = 201,   /* Illegal operation parameters */

    BEInvOpCode     = BEinoper,
    BEIOError       = BEioerr,
    BEFileNotOpen   = BEnoopen,
    BERecNotFound   = BEnorec,
    BEDupKey        = BEdupkey,
    BEInvKeyNum     = BEinvkey,
    BEKeyNumChanged = BEdifkey,
    BEInvPos        = BEinpos,
    BEOF            = BEeof,
    BEBadFileName   = BEfilenm,
    BEFileNotFound  = BEnofile,
    BEDiskFull      = BEfulld,
    BEKeyBufLen     = 21,
    BEDataBufLen    = BEldata,
    BEInvCreatData  = 24,
    BECantCreate    = 25,
    BEInvNumOfKeys  = 26,
    BEInvKeyPos     = 27,
    BEInvRecLen     = 28,
    BEInvKeyLen     = 29,
    BEDirError      = 35,
    BETrnError      = 36,
    BETrnActive     = 37,
    BETrnControl    = 38,
    BETrnInactive   = 39,
    BEOpNotAllowed  = 41,
    BEInvRecNum     = BEnumrec,
    BEInvKeyFlags   = 45,
    BENoWriteAccess = 46,
    BEInvKeyType    = 49,
    BEOwnerSet      = BEowner,
    BEInvOwner      = 51,
    BEFileExists    = BEefile,
    BEInternalErr   = 79,
    BELostPosition  = BElostpos,
    BEReadOutTrn    = 83,
    BERecLocked     = BErecloc,
    BEFileLocked    = BEfileloc,
    BEIncpmMode     = 88,
    BEAuthority     = 94,

    /* Extended errors */
    BEminexterr     = -16,
    BEnomoreclnts   = BEminexterr,  /* There are no more free client slots */
    BEillclntid,             /* Illegal client id */
    BEillclnth,              /* Illegal client handle */
    BEinitReq,               /* Can not init requester (standart or ours) */
    BEnodesc,                /* Description not found */
    BEmaxexterr
   };

// -----------------------------------------------
typedef enum BError BError;

// -----------------------------------------------
typedef enum BTableFlags
      {
       BTFVarLength      = 0x0001,
       BTFTruncBlanks    = 0x0002,
       BTFPreAll         = 0x0004,
       BTFDataCompressed = 0x0008,
       BTFKeyOnly        = 0x0010,
       BTFNdxBalanced    = 0x0020,
       BTFFree10         = 0x0040,
       BTFFree20         = 0x0080,
       BTFFree30         = BTFFree10 | BTFFree20,
       BTFExplicitKeyNum = 0x0400,
       BTFUseVAT         = 0x0800,

       BTFVLFlags        = BTFVarLength | BTFTruncBlanks | BTFUseVAT, /* Variable-length record flags */
       BTFInternalFlags  = BTFPreAll | BTFDataCompressed | BTFNdxBalanced | BTFFree30,

       BTFAllFlags       = BTFVLFlags | BTFInternalFlags | BTFKeyOnly | BTFExplicitKeyNum
      } BTableFlags;

typedef enum BKeyFlags
      {
       BKDuplicated   = 0x0001,
       BKModifiable   = 0x0002,
       BKBinary       = 0x0004,
       BKAllNull      = 0x0008,
       BKSegmented    = 0x0010,
       BKAlternate    = 0x0020,
       BKDescending   = 0x0040,
       BKRepDup       = 0x0080,
       BKExtended     = 0x0100,
       BKAnyNull      = 0x0200,
       BKNoCaseSense  = 0x0400,
       BKMultipleACS  = BKNoCaseSense | BKAlternate,
       BKNullValue    = BKAllNull | BKAnyNull,

       /* This flag set must be equal for all segments of multi-segment key */
       /* As a fact it flags of key, not segment */
       BKKeyFlags = BKDuplicated | BKModifiable | BKNullValue | BKRepDup,
       /* These flags can differ for every segment in key */
       BKSegFlags = BKAlternate | BKDescending | BKNoCaseSense | BKExtended
      } BKeyFlags;

/*******************************************************************************
            Compatibility enums
*******************************************************************************/
enum File_Flas
   {
    VarLen  = BTFVarLength,       /* Allow the file to contain variable length */
                                  /* records */
    TruncB  = BTFTruncBlanks,     /* Truncate trailing blanks in variable length */
                                  /* records */
    PreAll  = BTFPreAll,          /* Preallocate the number of pages you specify */
                                  /* in the allocation word */
    DataC   = BTFDataCompressed,  /* Compress the data in the file */
    KeyOnly = BTFKeyOnly,         /* Create the file as a key-only file */

    Free10  = BTFFree10,          /* Maintain a 10% free space threshold on the */
    Free20  = BTFFree20,          /* variable length pages */
    Free30  = BTFFree30
   };

enum Key_Flags
   {
    Kdup  = BKDuplicated,   /* The key allows duplicates */
    Kmod  = BKModifiable,   /* The key is modifiable */
    Kbin  = BKBinary,       /* The key is binary */
    Knull = BKAllNull,      /* The key has a null value */
    Ksegm = BKSegmented,    /* The key has another segment */
    Kalt  = BKAlternate,    /* The key is sorted by an alternate */
                            /* collation sequence */
    Kdes  = BKDescending,   /* The key is sorted in descending order */
    Ksup  = BKRepDup,       /* The key is supplemental stat operation only */
    Kext  = BKExtended,     /* The key is an extended type */
    Kman  = BKAnyNull       /* The key is manual */
   };

// -----------------------------------------------
enum
   {
    NotOverFile = -1,     /* Do not overwrite existing file */
    OverFile    = 0       /* Overwrite existing file */
   };


/*******************************************************************************
 Types
*******************************************************************************/

struct DBtrvFile;

// -----------------------------------------------
typedef unsigned long  recpos_t;
typedef unsigned long  chunkoffs_t;

// -----------------------------------------------
#define  RECPOS_T_ZERO      ((unsigned long)0)
#define  RECPOS_T_NOT_ZERO  ((unsigned long)1)

/* BTrieve client id type. Retrieved by getBtrvClient; released by releaseBtrvClient. */
/* Zero client id designates default client (i.e. same as no client id at all). */

typedef unsigned long   bclient_t;
typedef long            bclient_h;
typedef unsigned long   binstance_t;
typedef DBtrvFile      *btrvfile400_h;

/*******************************************************************************
 Структуры
*******************************************************************************/

/********************************* Version operation support ************/

#include <packpsh1.h>

/* ***** 1-BYTE PACKING */

typedef struct FileSpecs
      {
       unsigned short  reclen;    /* Record length */
       unsigned short  pagesize;  /* Page size, mult of 512 up to 4096 */
       unsigned short  nindex;    /* Number of indexes */
       unsigned long   nrecords;  /* Number of records for stat operation */
       unsigned short  fileflag;  /* File flags */
       unsigned char   dupptrs;
       unsigned char   reserved;
       unsigned short  prealloc;  /* Preallocation size in pages */
                                  /* Unused pages for stat operation */
      } FileSpecs;

// -----------------------------------------------
#define  BTRV_FILE_SPECS(reclen, pagesize, nindex, fileflag, prealloc) \
           {                             \
            (db_int16)(reclen),        \
            (db_int16)(pagesize),      \
            (db_int16)(nindex),        \
            (db_uint32)0,              \
            (db_int16)(fileflag),      \
            '\0',                      \
            '\0',                      \
            (db_int16)(prealloc)       \
           }

// -----------------------------------------------
typedef struct KeySpecs
      {
       unsigned short  keypos;    /* Segment offset from record begining */
       unsigned short  keylen;    /* Segment length */
       unsigned short  keyflag;   /* Segment flags */
       unsigned long   nkeys;     /* Number or unique key values for stat operation */
       unsigned char   keytype;   /* Extended key type */
       unsigned char   nullvalue;
       short           reserved;
       unsigned char   keynum;
       unsigned char   ACS;
      } KeySpecs ;

// -----------------------------------------------
#define  BTRV_KEY_SPECS(keypos, keylen, keyflag, keytype, nullvalue) \
         {                             \
          (db_int16)(keypos),        \
          (db_int16)(keylen),        \
          (db_int16)(keyflag),       \
          (db_uint32)0,              \
          (keytype),                 \
          (nullvalue),               \
          0,                         \
          '\0',                      \
          '\0'                       \
         }

// -----------------------------------------------
typedef struct AltCol
      {
       char           sign;       /* Signature byte 0xAC */
       char           name[8];    /* Uniquely sequence name */
       unsigned char  tab[256];   /* Translation table */
      } AltCol;

// -----------------------------------------------
typedef struct VersBuf
      {
       short int  version;  /* Version number */
       short int  revision; /* Revision number */
       char       netware;  /* 'N' for NetWare Btrive */
      } VersBuf;

// -----------------------------------------------
typedef struct FileSpecsEx
      {
       unsigned   maxvarlen;    /* Max length of bytes variable part */
       char       blob;         /* Variable part usage as BLOB */
       short int  maxrecwait;   /* Maximum record wait time */
       char       reserved[9];
      } FileSpecsEx;

// -----------------------------------------------
typedef struct BTClientID
      {
       char            reserv[12];
       char            id1[2];
       unsigned short  id2;
      } BTClientID;

/*******************************************************************************
                        struct bposblk

 BTRIEVE emulator positional block. Contains CRC32 for error proof
*******************************************************************************/
#define  POS_BLK_MAGIC  0x031B07AD

typedef struct bposblk
      {
       void          *reserved1[3]; /* For the case... */

       btrvfile400_h  handle;       /* Pointer to file object */
       int            keynum;       /* Current key number */
       unsigned long  flags;        /* Flags of last executed operation */
       bclient_h      client;       /* Positional block's client */
       recpos_t       pos;          /* Last physical position */
       chunkoffs_t    cpos;         /* Last in-record position */
       binstance_t    krnlinst;     /* Number of BTrieve kernel activation (instance) */
       binstance_t    clntinst;     /* Number of current client instance */
       binstance_t    pblkinst;     /* Number of current positional block instance */
       binstance_t    trninst;      /* Transaction instance for last read operation */
       unsigned long  magic;        /* "Magic number" */
       unsigned long  crc32;        /* CRC32 of positional block. CRC32 contains positional block */
                                    /* address in memory; so, positional block is not movable */
      } bposblk;

/* ***** END OF 1-BYTE PACKING */

#include <packpop.h>

// -----------------------------------------------
typedef bposblk   posblk_t;
typedef bposblk  *posblk_h;

// -----------------------------------------------
#define  BCLIENT_BAD_HANDLE  ((bclient_h)-1)
#define  BCLIENT_DEF_HANDLE  ((bclient_h)0)
#define  BCLIENT_BAD_CLIENT  ((bclient_t)0xFFFFFFFEUL)

/*
 *  btrvOpName    -  BTrieve operation name
 */
const char *btrvOpName(Btrieve_Operations op);

/*
 *  btrvErrorText -  Text message for BTrieve error code
 */
const char *btrvErrorText(BError errorCode);

/*
 * THE MAIN FUNCTION
 */
BError NWcallEx(int op, posblk_h posblk, void *databuf, int *datalen, void *keybuf, int keylen,
                signed char keynum, bclient_h client_id);

// -----------------------------------------------
#define  NWcall(op,posblk,databuf,datalen,keybuf,keylen,keynum) \
              NWcallEx((op), (posblk), (databuf), (datalen), (keybuf), (keylen), (keynum), 0)


/*******************************************************************************
   Some BTrieve client stuff.
   Really stupid stubs
*******************************************************************************/
bclient_t btrvHandle2Id(bclient_h handle);
bclient_h bGetClient(const char *name, int *stat);
int       bReleaseClient(bclient_h client);
bclient_h bSetDefClient(bclient_h client, int *stat);
bclient_t btrvGetDefClient(void);
bclient_h btrvId2Handle(bclient_t id);
void      btrvNuke(void);

inline int getFullClientID(BTClientID *id, bclient_h client)
{
 return -1;
}


/*******************************************************************************
  Support functions
*******************************************************************************/
_rs_inline_ const KeySpecs *keyspec_end(const KeySpecs *begin);
   
_rs_inline_ const KeySpecs *keyseg_range(const KeySpecs *begin, int keynum, const KeySpecs **end);

/*
** In C++ mode all these function are inlines.
** In C mode they are normal functions
*/

#if defined(__cplusplus) && !defined(__NWBTRV_CC)
#include <nwbtrv.cc>
#endif /* NWBTRV.CC */


/*******************************************************************************
 Chunk operations
*******************************************************************************/
// -----------------------------------------------
typedef struct tagTChunkDes
  {
   chunkoffs_t  ChunkOffset;
   chunkoffs_t  ChunkLen;
   void        *UserData;
  } TChunkDes;

// -----------------------------------------------
typedef struct TChunkHeader
      {
       unsigned long  Subfunction;
       unsigned long  NumChunks;
      } TChunkHeader;

// -----------------------------------------------
typedef struct TChunk
      {
       TChunkHeader  head;
       TChunkDes     des[1];
      } TChunk;

// -----------------------------------------------
typedef struct TReadChunk
      {
       unsigned long  addr;
       TChunkHeader   head;
       TChunkDes      des[1];
      } TReadChunk;

// -----------------------------------------------
typedef struct TWriteChunk
      {
       TChunkHeader   head;
       TChunkDes      des[1];
      } TWriteChunk;

// -----------------------------------------------
typedef struct TTruncateChunk
      {
       unsigned long  Subfunction;
       chunkoffs_t    ChunkOffset;
      } TTruncateChunk;

// -----------------------------------------------
#define  BT_DIRECT_RANDOM    0x80000000UL
#define  BT_INDIRECT_RANDOM  0x80000001UL

#define  BT_DIRECT_RECT      0x80000002UL
#define  BT_INDIRECT_RECT    0x80000003UL

#define  BT_TRUNCATE         0x80000004UL

#define  BT_NEXT_BIAS        0x40000000UL
#define  BT_APPEND_BIAS      0x20000000UL

#define  BT_INDIRECT_BIT     0x00000001UL

/*-@@------ END __ Chunk operations____________________________ --------@E- */

/*******************************************************************************
 Support operations - copy, rename, clone, move, delete DBtriever files
*******************************************************************************/
/*
 * Support operation flags
 */
#define  BTOP_FILEDATA   (0x0001)
#define  BTOP_REPLACE    (0x0002)

/*
 * btclone     -  clone file, copy file, clear file contents
 *
 * Parameters
 *
 */
BError btclone(const char *fromfile, const char *tofile, int flags);

/*
 * btrenmove   -  move file, rename file, delete file
 *
 * Parameters:
 *
 */
BError btrenmove(const char *fromfile, const char *tofile, int flags);

#ifdef __cplusplus
}
#endif

#endif /* __NWBTRV_H */

/* <-- EOF --> */