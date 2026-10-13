/*******************************************************************************
 FILE         :   KEYS.H

 COPYRIGHT    :   1991 - 1994 by R-Style JSC
                  R-Style Software Lab, 1995-1998

 DESCRIPTION  :   Содержит коды клавиш

 PROGRAMMED BY:   Serge Kubrin (K78)

 CREATION DATE:   9 Sep 1998

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/h/keys.h $
                  $Revision: 12 $
                  $Date: 28.04.00 14:04 $
                  $Author: Kubrin $
                  $Modtime: 28.04.00 14:04 $
*******************************************************************************/
/*-*- History -------------------------------------------------------------*-
 January 6,1995   Sergei Kubrin (K78) - Create file
 March 10,1995   Serg Kubrin (K78)
            Add TAllKeys and TShiftFlags
-*-----------------------------------------------------------------------*/

#ifndef __KEYS_H
#define __KEYS_H

#include <platform.h>
#include <pexpdef.h>

#define  K_ASCII_CTRL_BS   0x007F
#define  K_EBCDIC_CTRL_BS  0x003F

#if defined(RSL_CHAR_ASCII)
    #define  RSL_CTRL_BS  K_ASCII_CTRL_BS
#elif defined(RSL_CHAR_EBCDIC)
    #define  RSL_CTRL_BS  K_EBCDIC_CTRL_BS
#else
    #error "Not defined character table: RSL_CHAR_ASCII or RSL_CHAR_EBCDIC."
#endif

/*******************************************************************************
                           enum TShiftFlags
 Keyboard shift flags
*******************************************************************************/
typedef enum TShiftFlags
      {
       SH_NONE          =    0,
       SH_SHIFT         =    3,
       SH_CTRL          =    4,
       SH_ALT           =    8,
       SH_SCROLL        =   16,
       SH_NUMLOCK       =   32,
       SH_CAPSLOCK      =   64,

       SH_RSHIFT        =  128,
       SH_RCTRL         =  256,
       SH_RALT          =  512,

       SH_UNGKEY        = 1024,
       SH_TESTKB        = 2048,
      } TShiftFlags;

#define  SH_CONTROL    SH_CTRL
#define  SH_RCONTROL   SH_RCTRL

typedef enum TScrollFlags
      {
       SCROLL_LINEUP     = 0,
       SCROLL_LINELEFT   = 0,
       SCROLL_LINEDOWN   = 1,
       SCROLL_LINERIGHT  = 1,
       SCROLL_PAGEUP     = 2,
       SCROLL_PAGELEFT   = 2,
       SCROLL_PAGEDOWN   = 3,
       SCROLL_PAGERIGHT  = 3,
       SCROLL_TRACKED    = 4,
       SCROLL_TRACKING   = 5,
       SCROLL_TOP        = 6,
       SCROLL_LEFT       = 6,
       SCROLL_BOTTOM     = 7,
       SCROLL_RIGHT      = 7,
       SCROLL_ENDS       = 8,
       SCROLL_BEGIN      = 9
      } TScrollFlags;

/*******************************************************************************
                           enum TAllKeys
 Keyboard codes (returned by fs_getkbd)
*******************************************************************************/

//  Коды соответсвующие нажатию клавиш или их комбинаций попадают в диапазон
//  1 - 420. Исключение комбинация CtrlAltF7 - 768

//  Коды соответсвующие событиям мыши попадают в диапазон 513 - 521, 511

//  Системные сообщения попадают в диапазон 522 - 532. Также используются коды
//  256 (не пересекается с клавиатурными кодами), 500, 512, 769 - 771, 1000

typedef enum TAllKeys
      {
       EMPTYKEY         = 0,

       // Keyboard codes
       K_BS             = 0x0008,     // 8
       K_TAB            = 0x0009,     // 9

       K_CTRLENTER      = 0x000A,     // 10

       K_ENTER          = 0x000D,     // 13

       K_CTRLA          = 0x0001,     // 1
       K_CTRLB          = 0x0002,     // 2
       K_CTRLC          = 0x0003,     // 3
       K_CTRLD          = 0x0004,     // 4
       K_CTRLE          = 0x0005,     // 5
       K_CTRLF          = 0x0006,     // 6
       K_CTRLG          = 0x0007,     // 7
       K_CTRLH          = 0x0008,     // 8
       K_CTRLI          = 0x0009,     // 9
       K_CTRLJ          = 0x000A,     // 10
       K_CTRLK          = 0x000B,     // 11
       K_CTRLL          = 0x000C,     // 12
       K_CTRLM          = 0x000D,     // 13
       K_CTRLN          = 0x000E,     // 14
       K_CTRLO          = 0x000F,     // 15
       K_CTRLP          = 0x0010,     // 16
       K_CTRLQ          = 0x0011,     // 17
       K_CTRLR          = 0x0012,     // 18
       K_CTRLS          = 0x0013,     // 19
       K_CTRLT          = 0x0014,     // 20
       K_CTRLU          = 0x0015,     // 21
       K_CTRLV          = 0x0016,     // 22
       K_CTRLW          = 0x0017,     // 23
       K_CTRLX          = 0x0018,     // 24
       K_CTRLY          = 0x0019,     // 25
       K_CTRLZ          = 0x001A,     // 26

       K_ESC            = 0x001B,     // 27
       K_LAST_ASCII_CTRL = K_ESC,

       K_CTRL6          = 0x001E,     // 30
       K_CTRLMIN        = 0x001F,     // 31

       K_CTRLBS         = RSL_CTRL_BS,// 63 | 127

       K_CTRLLT         = 0x00E8,     // 232

       K_ALTCOL         = 0x00E9,     // 233
       K_ALTCBR         = 0x00EA,     // 234
       K_ALTOBR         = 0x00EC,     // 236
       K_ALTCOM         = 0x00ED,     // 237

       K_CTRLGR         = 0x00EF,     // 239

       K_CTRL0          = 0x0101,     // 257
       K_CTRL1          = 0x0102,     // 258
       K_CTRL2          = 0x0103,     // 259
       K_CTRL3          = 0x0104,     // 260
       K_CTRL4          = 0x0105,     // 261
       K_CTRL5          = 0x0106,     // 262
       K_CTRL7          = 0x0107,     // 263
       K_CTRL8          = 0x0108,     // 264
       K_CTRL9          = 0x0109,     // 265
       K_CTRLPLUS       = 0x010A,     // 266

       K_ALTBS          = 0x010E,     // 270
       K_SHTAB          = 0x010F,     // 271

       K_ALTQ           = 0x0110,     // 272
       K_ALTW           = 0x0111,     // 273
       K_ALTE           = 0x0112,     // 274
       K_ALTR           = 0x0113,     // 275
       K_ALTT           = 0x0114,     // 276
       K_ALTY           = 0x0115,     // 277
       K_ALTU           = 0x0116,     // 278
       K_ALTI           = 0x0117,     // 279
       K_ALTO           = 0x0118,     // 280
       K_ALTP           = 0x0119,     // 281
       K_ALTLSQBR       = 0x011A,     // 282
       K_ALTRSQBR       = 0x011B,     // 283

       K_ALTA           = 0x011E,     // 286
       K_ALTS           = 0x011F,     // 287
       K_ALTD           = 0x0120,     // 288
       K_ALTF           = 0x0121,     // 289
       K_ALTG           = 0x0122,     // 290
       K_ALTH           = 0x0123,     // 291
       K_ALTJ           = 0x0124,     // 292
       K_ALTK           = 0x0125,     // 293
       K_ALTL           = 0x0126,     // 294

       K_ALTZ           = 0x012C,     // 300
       K_ALTX           = 0x012D,     // 301
       K_ALTC           = 0x012E,     // 302
       K_ALTV           = 0x012F,     // 303
       K_ALTB           = 0x0130,     // 304
       K_ALTN           = 0x0131,     // 305
       K_ALTM           = 0x0132,     // 306
       K_ALTLT          = 0x0133,     // 307
       K_ALTGR          = 0x0134,     // 308

       K_ALTGRAST       = 0x0137,     // 311

       K_F1             = 0x013B,     // 315
       K_F2             = 0x013C,     // 316
       K_F3             = 0x013D,     // 317
       K_F4             = 0x013E,     // 318
       K_F5             = 0x013F,     // 319
       K_F6             = 0x0140,     // 320
       K_F7             = 0x0141,     // 321
       K_F8             = 0x0142,     // 322
       K_F9             = 0x0143,     // 323
       K_F10            = 0x0144,     // 324

       K_HOME           = 0x0147,     // 327
       K_UP             = 0x0148,     // 328
       K_PGUP           = 0x0149,     // 329
       K_ALTGRMIN       = 0x14A,      // 330
       K_LEFT           = 0x014B,     // 331
       K_RIGHT          = 0x014D,     // 333
       K_ALTGRPLUS      = 0x014E,     // 334
       K_END            = 0x014F,     // 335
       K_DOWN           = 0x0150,     // 336
       K_PGDN           = 0x0151,     // 337
       K_INS            = 0x0152,     // 338
       K_DEL            = 0x0153,     // 339

       K_SHF1           = 0x0154,     // 340
       K_SHF2           = 0x0155,     // 341
       K_SHF3           = 0x0156,     // 342
       K_SHF4           = 0x0157,     // 343
       K_SHF5           = 0x0158,     // 344
       K_SHF6           = 0x0159,     // 345
       K_SHF7           = 0x015A,     // 346
       K_SHF8           = 0x015B,     // 347
       K_SHF9           = 0x015C,     // 348
       K_SHF10          = 0x015D,     // 349

       K_CTRLF1         = 0x015E,     // 350
       K_CTRLF2         = 0x015F,     // 351
       K_CTRLF3         = 0x0160,     // 352
       K_CTRLF4         = 0x0161,     // 353
       K_CTRLF5         = 0x0162,     // 354
       K_CTRLF6         = 0x0163,     // 355
       K_CTRLF7         = 0x0164,     // 356
       K_CTRLF8         = 0x0165,     // 357
       K_CTRLF9         = 0x0166,     // 358
       K_CTRLF10        = 0x0167,     // 359

       K_ALTF1          = 0x0168,     // 360
       K_ALTF2          = 0x0169,     // 361
       K_ALTF3          = 0x016A,     // 362
       K_ALTF4          = 0x016B,     // 363
       K_ALTF5          = 0x016C,     // 364
       K_ALTF6          = 0x016D,     // 365
       K_ALTF7          = 0x016E,     // 366
       K_ALTF8          = 0x016F,     // 367
       K_ALTF9          = 0x0170,     // 368
       K_ALTF10         = 0x0171,     // 369

       K_CTRLLEFT       = 0x0173,     // 371
       K_CTRLRIGHT      = 0x0174,     // 372
       K_CTRLEND        = 0x0175,     // 373
       K_CTRLPGDN       = 0x0176,     // 374
       K_CTRLHOME       = 0x0177,     // 375

       K_ALT1           = 0x0178,     // 376
       K_ALT2           = 0x0179,     // 377
       K_ALT3           = 0x017A,     // 378
       K_ALT4           = 0x017B,     // 379
       K_ALT5           = 0x017C,     // 380
       K_ALT6           = 0x017D,     // 381
       K_ALT7           = 0x017E,     // 382
       K_ALT8           = 0x017F,     // 383
       K_ALT9           = 0x0180,     // 384
       K_ALT0           = 0x0181,     // 385
       K_ALTMIN         = 0x0182,     // 386
       K_ALTPLUS        = 0x0183,     // 387

       K_CTRLPGUP       = 0x0184,     // 388

       K_F11            = 0x0185,     // 389
       K_F12            = 0x0186,     // 390

       K_SHF11          = 0x0187,     // 391
       K_SHF12          = 0x0188,     // 392

       K_CTRLF11        = 0x0189,     // 393
       K_CTRLF12        = 0x018A,     // 394

       K_ALTF11         = 0x018B,     // 395
       K_ALTF12         = 0x018C,     // 396

       K_CTRLUP         = 0x018D,     // 397

       K_CTRLGRMIN      = 0x18E,      // 398

       K_CTRLGRPLUS     = 0x190,      // 400

       K_CTRLDOWN       = 0x0191,     // 401
       K_CTRLINS        = 0x0192,     // 402
       K_CTRLDEL        = 0x0193,     // 403
       K_CTRLTAB        = 0x0194,     // 404

       K_CTRLGRSLASH    = 0x0195,     // 405
       K_CTRLGRAST      = 0x0196,     // 406

       K_ALTHOME        = 0x0197,     // 407
       K_ALTUP          = 0x0198,     // 408
       K_ALTPGUP        = 0x0199,     // 409
       K_ALTLEFT        = 0x019B,     // 411
       K_ALTRIGHT       = 0x019D,     // 413
       K_ALTEND         = 0x019F,     // 415
       K_ALTDOWN        = 0x01A0,     // 416
       K_ALTPGDN        = 0x01A1,     // 417
       K_ALTINS         = 0x01A2,     // 418
       K_ALTDEL         = 0x01A3,     // 419

       K_ALTGRSLASH     = 0x01A4,     // 420

       PREKEY           = 512,

       //  Mouse codes
       K_PRESS          = 513,
       K_APRESS         = 514,
       K_DEPRESS        = 515,
       K_MOVE           = 516,
       K_PRESSR         = 517,
       K_DEPRESSR       = 518,
       K_MOVER          = 519,
       K_DBLCLICK       = 520,
       K_WHEELUP        = 521,
       K_WHEELDN        = 511,
       K_PRESSM         = 508,
       K_DEPRESSM       = 509,
       K_MOVEM          = 510,

       K_MINMOUSE       = K_PRESSM,
       K_MAXMOUSE       = K_WHEELUP,

       //  System messages
       WRONGKEY         = 256,

       K_RESIZE         = 500,
       K_MENU           = 501,
       K_BUTTON         = 502,
       K_HSCROLL        = 503,
       K_VSCROLL        = 504,
       K_HEADER         = 505,
    // K_RESERV  = 506,
    // K_RESERV2 = 507,

       ENTKEY           = 522,
       EXITKEY          = 523,

       POSTKEY          = 525,
       MSELPREKEY       = 526,
       MSELPOSTKEY      = 527,

       ENTFIELD         = 529,
       EXITFIELD        = 530,
       PREREC           = 531,
       POSTREC          = 532,

       K_TABSWITCH      = 550,        // Для переключения закладок в Tab Control-е.

       K_PICTURESOURCE  = 560,        // Для источника данных (Pictures)

       K_STATUSBAR,                   // Для секций статус-строки

       K_AUDIOSOURCE,                 // Для источника данных (Audio)

       K_CTRLALTF1      = 0x02FA,     // 762
       K_CTRLALTF2      = 0x02FB,     // 763
       K_CTRLALTF3      = 0x02FC,     // 764
       K_CTRLALTF4      = 0x02FD,     // 765
       K_CTRLALTF5      = 0x02FE,     // 766
       K_CTRLALTF6      = 0x02FF,     // 767
       K_CTRLALTF7      = 0x0300,     // 768
       K_CTRLALTF8      = 0x0301,     // 769
       K_CTRLALTF11     = 0x0304,     // 772
       K_CTRLALTF12     = 0x0305,     // 773

       K_EDIT           = 0x0302,     // 770

       K_REFRESH        = 0x0306,     // 774
       K_REPEAT         = 0x0307,     // 775

       K_USER           = 1000
      } TAllKeys;


#ifndef K_ABORT
    #define  K_ABORT  (K_USER + 1)
#endif

#ifndef K_QUIT
    #define  K_QUIT   (K_USER + 10)
#endif

#ifndef K_KICK
    #define  K_KICK   (K_USER + 14)
#endif

/*******************************************************************************
               Old keyboard code enum for backward compatibility
*******************************************************************************/
enum KEYS
   {
    COMMAND    = 1000,
    MNCOMMAND  = 1001,

    PRESSM     = 508,
    DEPRESSM   = 509,
    MOVEM      = 510,

    PRESS      = 513,
    APRESS     = 514,
    DEPRESS    = 515,
    MOVE       = 516,
    PRESSR     = 517,
    DEPRESSR   = 518,
    MOVER      = 519,
    DBLCLICK   = 520,
    PRESS2     = 521,

    APRESS2    = 524,

    LAST2FIRST = 528,

    BS         =   8,
    TAB        =   9,
    ENTER      =  13,
    ESC        =  27,
    SHFTAB     = 271,
    DELLINE    = 288,
    BSLINE     = -10,

    F1         = 315,
    F2         = 316,
    F3         = 317,
    F4         = 318,
    F5         = 319,
    F6         = 320,
    F7         = 321,
    F8         = 322,
    F9         = 323,
    F10        = 324,
    tk1        = 325,
    tk2        = 326,
    HOME       = 327,
    UP         = 328,
    PGUP       = 329,
    GRMINUS    = 330,
    LEFT       = 331,
    PAD5       = 332,
    RIGHT      = 333,
    GRPLUS     = 334,
    END        = 335,
    DOWN       = 336,
    PGDOWN     = 337,
    INS        = 338,
    DEL        = 339,
    SHF1       = 340,
    SHF2       = 341,
    SHF3       = 342,
    SHF4       = 343,
    SHF5       = 344,
    SHF6       = 345,
    SHF7       = 346,
    SHF8       = 347,
    SHF9       = 348,
    SHF10      = 349,
    CTRLF1     = 350,
    CTRLF2     = 351,
    CTRLF3     = 352,
    CTRLF4     = 353,
    CTRLF5     = 354,
    CTRLF6     = 355,
    CTRLF7     = 356,
    CTRLF8     = 357,
    CTRLF9     = 358,
    CTRLF10    = 359,
    ALTF1      = 360,
    ALTF2      = 361,
    ALTF3      = 362,
    ALTF4      = 363,
    ALTF5      = 364,
    ALTF6      = 365,
    ALTF7      = 366,
    ALTF8      = 367,
    ALTF9      = 368,
    ALTF10     = 369,
    tk3        = 370,
    CTRLLEFT   = 371,
    CTRLRIGHT  = 372,
    CTRLEND    = 373,
    CTRLPGDOWN = 374,
    CTRLHOME   = 375,
    ALT1       = 376,
    ALT2       = 377,
    ALT3       = 378,
    ALT4       = 379,
    ALT5       = 380,
    ALT6       = 381,
    ALT7       = 382,
    ALT8       = 383,
    ALT9       = 384,
    ALT0       = 385,
    ALTMINUS   = 386,
    ALTEQ      = 387,
    CTRLPGUP   = 388,
    F11        = 389,
    F12        = 390,
    SHF11      = 391,
    SHF12      = 392,
    CTRLF11    = 393,
    CTRLF12    = 394,
    ALTF11     = 395,
    ALTF12     = 396
   };

#define  DBLKLICK  DBLCLICK
#define  DPRESS    DBLCLICK
#define  K_DPRESS  DBLCLICK

//
// Pseudo-graphics characters
//
extern const unsigned char RSL_FAR _RSPNLEXP  rslGraphC[];

/*******************************************************************************
                     enum RslGraphChar

 Indexes of pseudo-graphics character in rslGraphC array.
 Naming rules:
   Bar_25   - character-cell-sized bar filled by 25%
   Bar_Full - character-cell-sized fully filled bar
   Bar_Down - half-character-cell-sized fully filled bar at lower half of char cell.
   Bar_Left - half-character-cell-sized fully filled bar at left half of char cell.

                                                                       |
   NWS_S    - North-West-South, single line                          --|
                                                                       |

                                                                       |
   NWS_SD   - North-West-South, single vertical, double horisontal   ==|
                                                                       |

   NW_D     - North-West, double line                                ==||
                                                                       ||

*******************************************************************************/
typedef enum RslGraphChar
      {
       RslGCharBar_25,
       RslGCharBar_50,
       RslGCharBar_75,
       RslGCharNS_S,
       RslGCharNWS_S,
       RslGCharNWS_SD,
       RslGCharNWS_DS,
       RslGCharSW_DS,
       RslGCharSW_SD,
       RslGCharNWS_D,
       RslGCharNS_D,
       RslGCharSW_D,
       RslGCharNW_D,
       RslGCharNW_DS,
       RslGCharNW_SD,
       RslGCharSW_S,
       RslGCharNE_S,
       RslGCharNWE_S,
       RslGCharSWE_S,
       RslGCharNES_S,
       RslGCharWE_S,
       RslGCharX_S,
       RslGCharNES_SD,
       RslGCharNES_DS,
       RslGCharNE_D,
       RslGCharSE_D,
       RslGCharNWE_D,
       RslGCharSWE_D,
       RslGCharNES_D,
       RslGCharWE_D,
       RslGCharX_D,
       RslGCharNWE_SD,
       RslGCharNWE_DS,
       RslGCharSWE_SD,
       RslGCharSWE_DS,
       RslGCharNE_DS,
       RslGCharNE_SD,
       RslGCharSE_SD,
       RslGCharSE_DS,
       RslGCharX_DS,
       RslGCharX_SD,
       RslGCharNW_S,
       RslGCharSE_S,
       RslGCharBar_Full,
       RslGCharBar_Down,
       RslGCharBar_Left,
       RslGCharBar_Right,
       RslGCharBar_Upper
      } RslGraphChar;


/*******************************************************************************
   Macro definitions for pseudo-graphics character.

   USE INSTEAD OF CHARACTER CODES!!!
*******************************************************************************/
#define  RSL_GC(n) ((unsigned char)(rslGraphC[(n)]))

#define  RSL_GC_BAR_25     RSL_GC(RslGCharBar_25)
#define  RSL_GC_BAR_50     RSL_GC(RslGCharBar_50)
#define  RSL_GC_BAR_75     RSL_GC(RslGCharBar_75)
#define  RSL_GC_NS_S       RSL_GC(RslGCharNS_S)
#define  RSL_GC_NWS_S      RSL_GC(RslGCharNWS_S)
#define  RSL_GC_NWS_SD     RSL_GC(RslGCharNWS_SD)
#define  RSL_GC_NWS_DS     RSL_GC(RslGCharNWS_DS)
#define  RSL_GC_SW_DS      RSL_GC(RslGCharSW_DS)
#define  RSL_GC_SW_SD      RSL_GC(RslGCharSW_SD)
#define  RSL_GC_NWS_D      RSL_GC(RslGCharNWS_D)
#define  RSL_GC_NS_D       RSL_GC(RslGCharNS_D)
#define  RSL_GC_SW_D       RSL_GC(RslGCharSW_D)
#define  RSL_GC_NW_D       RSL_GC(RslGCharNW_D)
#define  RSL_GC_NW_DS      RSL_GC(RslGCharNW_DS)
#define  RSL_GC_NW_SD      RSL_GC(RslGCharNW_SD)
#define  RSL_GC_SW_S       RSL_GC(RslGCharSW_S)
#define  RSL_GC_NE_S       RSL_GC(RslGCharNE_S)
#define  RSL_GC_NWE_S      RSL_GC(RslGCharNWE_S)
#define  RSL_GC_SWE_S      RSL_GC(RslGCharSWE_S)
#define  RSL_GC_NES_S      RSL_GC(RslGCharNES_S)
#define  RSL_GC_WE_S       RSL_GC(RslGCharWE_S)
#define  RSL_GC_X_S        RSL_GC(RslGCharX_S)
#define  RSL_GC_NES_SD     RSL_GC(RslGCharNES_SD)
#define  RSL_GC_NES_DS     RSL_GC(RslGCharNES_DS)
#define  RSL_GC_NE_D       RSL_GC(RslGCharNE_D)
#define  RSL_GC_SE_D       RSL_GC(RslGCharSE_D)
#define  RSL_GC_NWE_D      RSL_GC(RslGCharNWE_D)
#define  RSL_GC_SWE_D      RSL_GC(RslGCharSWE_D)
#define  RSL_GC_NES_D      RSL_GC(RslGCharNES_D)
#define  RSL_GC_WE_D       RSL_GC(RslGCharWE_D)
#define  RSL_GC_X_D        RSL_GC(RslGCharX_D)
#define  RSL_GC_NWE_SD     RSL_GC(RslGCharNWE_SD)
#define  RSL_GC_NWE_DS     RSL_GC(RslGCharNWE_DS)
#define  RSL_GC_SWE_SD     RSL_GC(RslGCharSWE_SD)
#define  RSL_GC_SWE_DS     RSL_GC(RslGCharSWE_DS)
#define  RSL_GC_NE_DS      RSL_GC(RslGCharNE_DS)
#define  RSL_GC_NE_SD      RSL_GC(RslGCharNE_SD)
#define  RSL_GC_SE_SD      RSL_GC(RslGCharSE_SD)
#define  RSL_GC_SE_DS      RSL_GC(RslGCharSE_DS)
#define  RSL_GC_X_DS       RSL_GC(RslGCharX_DS)
#define  RSL_GC_X_SD       RSL_GC(RslGCharX_SD)
#define  RSL_GC_NW_S       RSL_GC(RslGCharNW_S)
#define  RSL_GC_SE_S       RSL_GC(RslGCharSE_S)
#define  RSL_GC_BAR_FULL   RSL_GC(RslGCharBar_Full)
#define  RSL_GC_BAR_DOWN   RSL_GC(RslGCharBar_Down)
#define  RSL_GC_BAR_LEFT   RSL_GC(RslGCharBar_Left)
#define  RSL_GC_BAR_RIGHT  RSL_GC(RslGCharBar_Right)
#define  RSL_GC_BAR_UPPER  RSL_GC(RslGCharBar_Upper)

#endif  // __KEYS_H

/* <-- EOF --> */