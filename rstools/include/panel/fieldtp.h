/*-----------------------------------------------------------------------*-

 File Name   : fieldtp.h 

 Sybsystem   : Btrieve  
 Description : 

 Source      : 

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 November 22,1995  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __FIELDTP_H
#define __FIELDTP_H

    #ifndef TFVT_DEFINED
    #define TFVT_DEFINED
        #if !defined(NO_OLD_FIELD_TYPES) && !defined(NO_OLD_FIELDS) && !defined(_INC_WINDOWS)
            enum FVT_OLD 
               {
                INT,
                LONG,
                FLOAT,
                FLOATG,
                DOUBLE,
                DOUBLEG,
                DOUBLEM,
                STRING,
                SNR,
                DATE,
                TIME,
                SHTM,                  /* Часы, минуты                                 */
                CHR,
                UCHR,
                LDMON,
                LDMONR,
                DOUBLER,
                LDOUBLE,

                NUMSTR,
                DECIMAL,

                MONEY_OLD,
                LMONEY_OLD,
                MONEYR_OLD,
                LMONEYR_OLD,

                BINARY,
                NUMERIC,
                BIGINT,
                PICTURE
               };
        #endif

        enum TFVT
           {              /* Новые типы значений                                */
            FT_INT16,         // 0
            FT_INT32,         // 1
            FT_FLOAT,         // 2
            FT_FLOATG,        // 3
            FT_DOUBLE,        // 4
            FT_DOUBLEG,       // 5

            #if !defined(NUMERIC_AS_MONEY)
                FT_MONEY,     // 6
            #else
                FT_DUMMY6,    // 6
            #endif

            FT_STRING,        // 7
            FT_SNR,           // 8
            FT_DATE,          // 9
            FT_TIME,          // 10
            FT_SHTM,          // 11
            FT_CHAR,          // 12
            FT_UCHAR,         // 13

            #if !defined(NUMERIC_AS_MONEY)
                FT_LMONEY,    // 14
                FT_LMONEYR,   // 15
                FT_MONEYR,    // 16
            #else
                FT_DUMMY14,   // 14
                FT_DUMMY15,   // 15
                FT_DUMMY16,   // 16
            #endif

            FT_LDOUBLE10,     // 17
            
            FT_NUMSTR,        // 18

           #if !defined(NUMERIC_AS_MONEY)
                FT_DECIMAL,   // 19
           #else
                FT_DUMMY19,   // 19
           #endif

            FT_MONEY_OLD,     // 20
            FT_LMONEY_OLD,    // 21
            FT_MONEYR_OLD,    // 22
            FT_LMONEYR_OLD,   // 23

            FT_BINARY,        // 24
            FT_NUMERIC,       // 25
            //FT_BTRDEC,        // 26   // Btrieve decimal type (15 bytes) Used only as Btriev Data field
            FT_INT64,         // 27
            FT_PICTURE,       // 28

            FT_NATIVE = 0x80,

            #if !defined(SQLBUILD)
                FT_LDOUBLE = FT_LDOUBLE10,
            #else
                FT_LDOUBLE = FT_DOUBLE,
            #endif

            #if defined(USE_FDECIMAL)
                FT_FMONEY   = 14,
                FT_FMONEYR  = 15,
                FT_FDECIMAL = 19,
            #endif

            #if defined(NUMERIC_AS_MONEY)
                FT_MONEY   = FT_NUMERIC,
                FT_MONEYR  = FT_NUMERIC,
                FT_LMONEY  = FT_NUMERIC,
                FT_LMONEYR = FT_NUMERIC,
                FT_DECIMAL = FT_NUMERIC,
            #endif

            FT_INT16_NATIVE     = FT_INT16     | FT_NATIVE,
            FT_INT32_NATIVE     = FT_INT32     | FT_NATIVE,
            FT_FLOAT_NATIVE     = FT_FLOAT     | FT_NATIVE,
            FT_FLOATG_NATIVE    = FT_FLOATG    | FT_NATIVE,
            FT_DOUBLE_NATIVE    = FT_DOUBLE    | FT_NATIVE,
            FT_DOUBLEG_NATIVE   = FT_DOUBLEG   | FT_NATIVE,
            FT_LDOUBLE_NATIVE   = FT_LDOUBLE   | FT_NATIVE,
            FT_LDOUBLE10_NATIVE = FT_LDOUBLE10 | FT_NATIVE,
            FT_MONEY_NATIVE     = FT_MONEY     | FT_NATIVE,
            FT_MONEYR_NATIVE    = FT_MONEYR    | FT_NATIVE,
            FT_LMONEY_NATIVE    = FT_LMONEY    | FT_NATIVE,
            FT_LMONEYR_NATIVE   = FT_LMONEYR   | FT_NATIVE,
            FT_DECIMAL_NATIVE   = FT_DECIMAL   | FT_NATIVE,
            FT_NUMERIC_NATIVE   = FT_NUMERIC   | FT_NATIVE,
            FT_DATE_NATIVE      = FT_DATE      | FT_NATIVE,
            FT_TIME_NATIVE      = FT_TIME      | FT_NATIVE,
            FT_SHTM_NATIVE      = FT_SHTM      | FT_NATIVE,
            FT_INT64_NATIVE     = FT_INT64     | FT_NATIVE
           };

        #define  FT_INT             FT_INT16
        #define  FT_LONG            FT_INT32
        #define  FT_BIGINT          FT_INT64
        #define  FT_CHR             FT_CHAR
        #define  FT_UCHR            FT_UCHAR
        #define  FT_DOUBLEM         FT_MONEY
        #define  FT_DOUBLER         FT_MONEYR
        #define  FT_LDMON           FT_LMONEY
        #define  FT_LDMONR          FT_LMONEYR

        #define  FT_INT_NATIVE      FT_INT16_NATIVE
        #define  FT_LONG_NATIVE     FT_INT32_NATIVE
        #define  FT_BIGINT_NATIVE   FT_INT64_NATIVE
    #endif //TFVT_DEFINED

#endif //__FIELDTP_H

/* <-- EOF --> */