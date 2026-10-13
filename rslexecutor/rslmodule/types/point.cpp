#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsscript/registerobjlist.hpp"
#include "rslexecutor.h"
#include "rsscript/rslibdynamicfuncs.h"
#include <QPoint>

class QPointRsl
{
    //TGenObject* m_pRslObj;
public:
    QPoint pt;

    QPointRsl(TGenObject *pThis = NULL)
    {
    }

    ~QPointRsl()
    {

    };

    void Initialise(int firstParmOffs);

    RSL_CLASS(QPointRsl)
    RSL_INIT_DECL();

    RSL_GETPROP_DECL(x);
    RSL_PUTPROP_DECL(x);

    RSL_GETPROP_DECL(y);
    RSL_PUTPROP_DECL(y);

    RSL_GETPROP_DECL(transposed);
    RSL_GETPROP_DECL(isNull);
    RSL_GETPROP_DECL(manhattanLength);
};

void RegisterPointRsl()
{
    QPointRsl::TablePtr->clsName = "QPoint";
    _LibRslAddUniClass(QPointRsl::TablePtr, true);
}

Qt::HANDLE CreatePointRsl(const QPoint &pt)
{
    QPointRsl *pRsl = nullptr;
    TGenClass *cls = (TGenClass*)_LibRslFindUniClass(QPointRsl::TablePtr->clsName);
    TGenObject *obj = (TGenObject*)_LibCreateObjectOfClassEx(cls, NULL, (void**)&pRsl);
    pRsl->pt = pt;

    return obj;
}

bool IsPointRsl(Qt::HANDLE obj)
{
    return _LibRslGetUniClass(obj) == _LibRslFindUniClass("QPoint");
}

QPoint GetPointRsl(Qt::HANDLE obj)
{
    QPointRsl *_rsl = nullptr;
    _LibRslUniCast("QPoint", obj, (void**)&_rsl);

    if (!_rsl)
        return QPoint();

    return _rsl->pt;
}

RSL_CLASS_BEGIN(QPointRsl)
RSL_INIT
RSL_PROP_METH2(x)
RSL_PROP_METH2(y)
RSL_PROP_METH(transposed)
RSL_PROP_METH(isNull)
RSL_PROP_METH(manhattanLength)
RSL_CLASS_END;

void QPointRsl::Initialise(int firstParmOffs)
{
    enum
    {
        prm_x = 0,
        prm_y,
    };

    int ParamCount = GetFuncParamCount();
    if (ParamCount <= 1)
        return;

    int type = GetFuncParamType(prm_x + firstParmOffs);

    if (type != QVariant::Int)
        ThrowParamTypeError<int>(prm_x + firstParmOffs);

    type = GetFuncParamType(prm_y + firstParmOffs);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(prm_y + firstParmOffs);

    pt.setX(GetFuncParam(prm_x + firstParmOffs).toInt());
    pt.setY(GetFuncParam(prm_y + firstParmOffs).toInt());
}

RSL_INIT_IMP(QPointRsl)
{
    Initialise(*firstParmOffs);
}

RSL_GETPROP_IMP(QPointRsl, x)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        pt.x());
    return 0;
}

RSL_PUTPROP_IMP(QPointRsl, x)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    pt.setX(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QPointRsl, y)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        pt.y());
    return 0;
}

RSL_PUTPROP_IMP(QPointRsl, y)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    pt.setY(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QPointRsl, transposed)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        pt.transposed());
    return 0;
}

RSL_GETPROP_IMP(QPointRsl, isNull)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        pt.isNull());
    return 0;
}

RSL_GETPROP_IMP(QPointRsl, manhattanLength)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        pt.manhattanLength());
    return 0;
}
