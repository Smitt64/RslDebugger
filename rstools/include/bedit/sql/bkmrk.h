// -------------------------------------------------------------------------------------------------

#ifndef __BKMRK_H
#define __BKMRK_H

// -----------------------------------------------
#ifndef BKMHDEFINED
    #define BKMHDEFINED

    RSL_MAKE_HANDLE(BKMHANDLE);
#endif

extern "C" {

// -----------------------------------------------
#ifdef __TOOLS_BUILD
    #define RSDEMBK __declspec(dllexport)
#else
    #if !defined(EM_BUILD) && !defined(RSL_BUILD_DLL)
        #define  RSDEMBK  __declspec(dllimport)
    #else
        #define  RSDEMBK
    #endif
#endif  

// -----------------------------------------------
RSDEMBK int  releaseBmk  (BKMHANDLE pBmk);
RSDEMBK int  addRefBmk   (BKMHANDLE pBmk);   
RSDEMBK bool EmCompareBmk(BKMHANDLE left, BKMHANDLE right);

} // extern "C"

// -----------------------------------------------
typedef struct  tagBTFileInfo  BTFileInfo;

// -----------------------------------------------
class IBtBookmark
    {
     public:
       virtual int  __stdcall releaseBmk() = 0;
       virtual int  __stdcall addRefBmk()  = 0;   
       virtual bool __stdcall compareBmk(const IBtBookmark *ptr) const = 0;
    };

// -----------------------------------------------
class IBtUserDataSource
    {
     public:
       virtual void __stdcall onBfClose() = 0;

       virtual int __stdcall getBookmark(BKMHANDLE *handle, bool *isSimple) const = 0;
       virtual int __stdcall moveToBookmark(BKMHANDLE handle, int flags, void *DATA_BUF, int *DATA_LEN, 
                                            void *KEY_BUF, int KEY_LEN, signed char KEY_NUM) = 0;

       virtual int __stdcall makeOperation(int OP, int flags, void *DATA_BUF, int *DATA_LEN, void *KEY_BUF,
                                           int KEY_LEN, signed char KEY_NUM) = 0;

       virtual BTFileInfo*  __stdcall getMetaInfo() = 0;
       virtual void         __stdcall freeMetaInfo(BTFileInfo *info) = 0;
    };

// -----------------------------------------------
class Bookmark
    {
     protected:
       BKMHANDLE  handle;
       char       kind;
           
       // ----------------------------------------
       enum
          {
           BKM_SIMPLE, BKM_SQL, BKM_USER
          };

     public:
       // ----------------------------------------
       Bookmark() : handle(0), kind (BKM_SIMPLE) {}

       // ----------------------------------------
       Bookmark(const Bookmark &rbm) : handle(0), kind(BKM_SIMPLE)
       {
        *this = rbm;
       }

       // ----------------------------------------
      ~Bookmark()
       {
        release();
       }

       // ----------------------------------------
       Bookmark &operator = (const Bookmark &rbm)
       {
        if(this != &rbm)
         {
          release();

          handle = rbm.handle;
          kind   = rbm.kind;

          if(kind == BKM_SQL)
            addRefBmk (handle);
          else if(kind == BKM_USER)
            ((IBtBookmark *)handle)->addRefBmk();
         }

        return *this;
       }

       // ----------------------------------------
       bool operator == (const Bookmark &rbm) const
       {
        if(this == &rbm)
          return true;

        if(handle == rbm.handle)
          return true;

        if(kind == rbm.kind)
         {
          if(kind == BKM_SQL)
            return EmCompareBmk(handle, rbm.handle);

          if(kind == BKM_USER)
            return ((IBtBookmark *)handle)->compareBmk((IBtBookmark *)rbm.handle);
         }

        return false;
       }

       // ----------------------------------------
       bool operator != (const Bookmark &rbm) const
       {
        return !(*this == rbm);
       }

       // ----------------------------------------
       bool operator ! () const
       {
        return handle == NULL;
       }

       #ifdef SQLBUILD
           // ------------------------------------
           operator bool () const
           {
            return handle != NULL;
           }
       #else
           // ------------------------------------
           Bookmark(long pos) : handle((BKMHANDLE)(intptr_t)pos), kind(BKM_SIMPLE) {}

           // ------------------------------------
           operator long () const
           {
            return getLong();
           }

           // ------------------------------------
           bool operator == (long val) const
           {
            if((kind == BKM_SIMPLE) && ((intptr_t)handle == (intptr_t)val))
              return true;

            return false;
           }

           // ------------------------------------
           bool operator != (long val) const
           { 
            return !(*this == val);
           }
       #endif

       // ----------------------------------------
       // для того, чтобы можно было уменьшить счетчик для BTRVFILE.RecPos
       // для Bookmark BTRVFILE.RecPos не вызывается деструктор, так как 
       // память для него отводит calloc
       void release()   
       {
        if(kind == BKM_SQL)
          releaseBmk(handle);
        else if(kind == BKM_USER)
          ((IBtBookmark *)handle)->releaseBmk();

        handle = NULL;
        kind   = BKM_SIMPLE;
       }              

       // ----------------------------------------
       void setValue(long pos)
       {
        release();

        handle = (BKMHANDLE)(intptr_t)pos;
        kind   = BKM_SIMPLE;
       }

       // ----------------------------------------
       void setValue(BKMHANDLE handle_)
       {
        if(handle_ != handle)
         {
          release ();

          handle = handle_;
          kind   = BKM_SQL;

          addRefBmk(handle);
         }
       }

       // ----------------------------------------
       void setValueUser(BKMHANDLE handle_, bool isSimple, bool addRef)
       {
        if(handle_ != handle)
         {
          release();

          handle = handle_;
          kind   = isSimple ? BKM_SIMPLE : BKM_USER;

          if(kind == BKM_USER && addRef)
            ((IBtBookmark *)handle)->addRefBmk();
         }
       }

       // ----------------------------------------
       BKMHANDLE getHandle() const
       {
        return handle;
       }

       // ----------------------------------------
       long getLong() const
       {
        return (long)(intptr_t)handle;
       }
    };

// -----------------------------------------------
class NonZeroBookmark : public Bookmark
    {
     public:
       NonZeroBookmark()
       {
        handle = (BKMHANDLE)1;
        kind   = BKM_SIMPLE;
       }
    };

// -----------------------------------------------
#define  RECPOS_T_ZERO      (Bookmark())
#define  RECPOS_T_NOT_ZERO  (NonZeroBookmark())

// -----------------------------------------------
inline bool operator < (const Bookmark &b1, const Bookmark &b2)
{
 return b1.getHandle() < b2.getHandle();
}

// -----------------------------------------------
#if 0

#define  RECPOS_T_ZERO      (*Bookmark::zeroValue())
#define  RECPOS_T_NOT_ZERO  (*Bookmark::nonZeroValue())

class bm_item;

#include "packpshn.h"

/**************************************************/
class Bookmark
{
 private:
 
    bm_item *ptr;
 public:
    Bookmark();
    Bookmark(const Bookmark &rbm);
   ~Bookmark();

    Bookmark &operator = (const Bookmark &rbm);

    bool      operator == (const Bookmark &rbm);
    bool      operator != (const Bookmark &rbm); 
    bool      operator ! ();
    operator bool();

    long  getLen();

    void *val() const;

    bool  isSqlBm;

    void release(); // для того, чтобы можно было уменьшить счетчик для BTRVFILE.RecPos
                    // для Bookmark BTRVFILE.RecPos не вызывается деструктор, так как 
                    // память для него отводит calloc
    void setValue(long val);
    void setValue(bm_item *pbi);

    static const Bookmark *zeroValue();
    static const Bookmark *nonZeroValue();
};  

/**************************************************/
#include "packpop.h"
#endif // 0

#endif __BKMRK_H

/* <-- EOF --> */