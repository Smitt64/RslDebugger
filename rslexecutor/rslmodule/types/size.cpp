#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsscript/registerobjlist.hpp"
#include "rslexecutor.h"
#include "rsscript/rslibdynamicfuncs.h"
#include <QSize>

class QSizeRsl
{
    //TGenObject* m_pRslObj;
public:
    QSize size;

    QSizeRsl(TGenObject *pThis = NULL)
    {
    }

    ~QSizeRsl()
    {

    };

    void Initialise(int firstParmOffs);

    RSL_CLASS(QSizeRsl)
    RSL_INIT_DECL();

    RSL_GETPROP_DECL(width);
    RSL_PUTPROP_DECL(width);

    RSL_GETPROP_DECL(height);
    RSL_PUTPROP_DECL(height);
    //RSL_METHOD_DECL(last);
    //RSL_METHOD_DECL(prev);
};

void RegisterSizeRsl()
{
    QSizeRsl::TablePtr->clsName = "QSize";
    _LibRslAddUniClass(QSizeRsl::TablePtr, true);
}

Qt::HANDLE CreateSizeRsl(const QSize &sz)
{
    QSizeRsl *pRsl = nullptr;
    TGenClass *cls = (TGenClass*)_LibRslFindUniClass(QSizeRsl::TablePtr->clsName);
    TGenObject *obj = (TGenObject*)_LibCreateObjectOfClassEx(cls, NULL, (void**)&pRsl);
    pRsl->size = sz;

    return obj;
}

bool IsSizeRsl(Qt::HANDLE obj)
{
    return _LibRslGetUniClass(obj) == _LibRslFindUniClass("QSize");
}

QSize GetSizeRsl(Qt::HANDLE obj)
{
    QSizeRsl *_rsl = nullptr;
    _LibRslUniCast("QSize", obj, (void**)&_rsl);

    if (!_rsl)
        return QSize();

    return _rsl->size;
}

RSL_CLASS_BEGIN(QSizeRsl)
RSL_INIT
RSL_PROP_METH2(width)
RSL_PROP_METH2(height)
RSL_CLASS_END;

void QSizeRsl::Initialise(int firstParmOffs)
{
    enum
    {
        prm_width,
        prm_height
    };

    if (GetFuncParamCount() == 0)
        return;

    if (GetFuncParamType(firstParmOffs + prm_width) != QVariant::Int)
        ThrowParamTypeError<int>(firstParmOffs + prm_width);

    if (GetFuncParamType(firstParmOffs + prm_height) != QVariant::Int)
        ThrowParamTypeError<int>(firstParmOffs + prm_height);

    size = QSize(GetFuncParam(firstParmOffs + prm_width).toInt(),
                 GetFuncParam(firstParmOffs + prm_height).toInt());
}

RSL_INIT_IMP(QSizeRsl)
{
    Initialise(*firstParmOffs);
}

RSL_GETPROP_IMP(QSizeRsl, width)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        size.width());
    return 0;
}

RSL_PUTPROP_IMP(QSizeRsl, width)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    size.setWidth(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QSizeRsl, height)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        size.height());
    return 0;
}

RSL_PUTPROP_IMP(QSizeRsl, height)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    size.setHeight(GetFuncParam(1).toInt());

    return 0;
}
