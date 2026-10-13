// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef TRSBRSLINSTTMPL_HPP
#define TRSBRSLINSTTMPL_HPP

#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsl/krnlintf.h"
#include "rsl/objintf.hpp"
#include "conintf.hpp"
#include "playrep.h"
#include "statvars.h"
#include "rsscript/typeinfo_p.h"
#include "rsscript/registerinfobase.h"
#include "rslexecutor.h"

class TRsbParmForRsl: public TRSLValArray, public TRSLValRef
{
public:
   TRsbParmForRsl
   (
      size_t Size = 0,
      HRSLINST hInst = RslGetCurrentInst()
   );

   VALUE* getParmArray();

   operator VALUE* ();

};

TRsbParmForRsl::TRsbParmForRsl(size_t Size, HRSLINST hInst)
    : TRSLValArray(hInst, Size),
    TRSLValRef(this, m_ptrArray, getCvtBuff(0), -1)
{
}

TRsbParmForRsl::operator VALUE * ()
{
  return TRSLValRef::operator VALUE*();
}

VALUE* TRsbParmForRsl::getParmArray()
{
  return getCount() ? m_ptrArray + 1:NULL;
}

// Ўаблон дл¤ пополнени¤ оберток дл¤ работы с интерфейсом экземпл¤ра RSL
template<class RSLInstWrpType>
class TRsbRSLInstTmpl: public RSLInstWrpType
{
public:
   // ѕо умолчанию создаетс¤ дл¤ текущего экземпл¤ра RSL
   TRsbRSLInstTmpl(HRSLINST hInst = RslGetCurrentInst())
      :RSLInstWrpType(hInst)
   {
   }

   // ћетод используетс¤ дл¤ вызовов глобальных переменных,
   // процедур и классов, по их дескриптору предварительно полученных
   // при помощи функции RslGetInstSymbol().
   // ¬ариант дл¤ TRsbParmForRsl.
   bool RslCallInstSymbol(HRSLSYM hSym, int Code, TRsbParmForRsl& Parm)
   {
      return RSLInstWrpType::RslCallInstSymbol(hSym, Code, Parm.getCount(), Parm.getParmArray(), Parm);
   }

   // ћетод аналогичен методу RslCallInstSymbol, только
   // значение параметра code предопределено как RSL_DISP_GET_OR_RUN.
   // ¬ариант дл¤ TRsbParmForRsl.
   bool CallSymGetOrRun(HRSLSYM hSym, TRsbParmForRsl& Parm)
   {
      return RslCallInstSymbol(hSym, RSL_DISP_GET_OR_RUN, Parm);
   }

   // ћетод аналогичен методу RslCallInstSymbol, только
   // значение параметра code предопределено как RSL_DISP_RUN.
   // ¬ариант дл¤ TRsbParmForRsl.
   bool CallSymRun(HRSLSYM hSym, TRsbParmForRsl& Parm)
   {
      return RslCallInstSymbol(hSym, RSL_DISP_RUN, Parm);
   }

   // ћетод аналогичен методу RslCallInstSymbol, только
   // значение параметра code предопределено как RSL_DISP_GET.
   // ¬ариант дл¤ TRsbParmForRsl.
   bool CallSymGet(HRSLSYM hSym, TRsbParmForRsl& Parm)
   {
      return RslCallInstSymbol(hSym, RSL_DISP_GET, Parm);
   }

   // ћетод аналогичен методу RslCallInstSymbol, только
   // значение параметра code предопределено как RSL_DISP_SET.
   // ¬ариант дл¤ TRsbParmForRsl.
   bool CallSymSet(HRSLSYM hSym, TRsbParmForRsl& Parm)
   {
      return RslCallInstSymbol(hSym, RSL_DISP_SET, Parm);
   }

   // ћетод вызывает на выполнение макро процедуру sName.
   bool RunMacroName(const char* sName, TRsbParmForRsl& Parm)
   {
      bool bRet = false;
      HRSLSYM hSym = RSLInstWrpType::RslGetInstSymbol(sName);
      if(hSym)
      {
         bRet = CallSymRun(hSym, Parm);
      }
      return bRet;
   }
};

#define RslGetDataPassedToPlayRep( data )       \
(                                               \
   ((STD_USERDATA*)(data))->addExecData ?       \
         ((STD_USERDATA*)(data))->addExecData : \
         (data)                                 \
)

#endif // TRSBRSLINSTTMPL_HPP
