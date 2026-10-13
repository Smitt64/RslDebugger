#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsscript/registerobjlist.hpp"
#include "rslexecutor.h"
#include "rsscript/rslibdynamicfuncs.h"
#include <QRect>
#include <QDebug>

class QRectRsl
{
    //TGenObject* m_pRslObj;
public:
    QRect rect;

    QRectRsl(TGenObject *pThis = NULL)
    {
    }

    ~QRectRsl()
    {

    };

    void Initialise(int firstParmOffs);

    RSL_CLASS(QRectRsl)
    RSL_INIT_DECL();
    RSL_METHOD_DECL(contains);
    RSL_METHOD_DECL(intersected);
    RSL_METHOD_DECL(intersects);
    RSL_METHOD_DECL(moveBottom);
    RSL_METHOD_DECL(moveBottomLeft);
    RSL_METHOD_DECL(moveBottomRight);
    RSL_METHOD_DECL(moveCenter);
    RSL_METHOD_DECL(moveLeft);
    RSL_METHOD_DECL(moveRight);
    RSL_METHOD_DECL(moveTo);
    RSL_METHOD_DECL(moveTop);
    RSL_METHOD_DECL(moveTopLeft);
    RSL_METHOD_DECL(moveTopRight);
    RSL_METHOD_DECL(translate);
    RSL_METHOD_DECL(translated);
    RSL_METHOD_DECL(united);

    RSL_GETPROP_DECL(x);
    RSL_PUTPROP_DECL(x);

    RSL_GETPROP_DECL(y);
    RSL_PUTPROP_DECL(y);

    RSL_GETPROP_DECL(width);
    RSL_PUTPROP_DECL(width);

    RSL_GETPROP_DECL(height);
    RSL_PUTPROP_DECL(height);

    RSL_GETPROP_DECL(size);
    RSL_PUTPROP_DECL(size);

    RSL_GETPROP_DECL(isEmpty);
    RSL_GETPROP_DECL(isNull);
    RSL_GETPROP_DECL(isValid);

    RSL_GETPROP_DECL(bottom);
    RSL_PUTPROP_DECL(bottom);

    RSL_GETPROP_DECL(left);
    RSL_PUTPROP_DECL(left);

    RSL_GETPROP_DECL(top);
    RSL_PUTPROP_DECL(top);

    RSL_GETPROP_DECL(right);
    RSL_PUTPROP_DECL(right);

    RSL_GETPROP_DECL(center);

    RSL_GETPROP_DECL(bottomLeft);
    RSL_PUTPROP_DECL(bottomLeft);

    RSL_GETPROP_DECL(topRight);
    RSL_PUTPROP_DECL(topRight);

    RSL_GETPROP_DECL(bottomRight);
    RSL_PUTPROP_DECL(bottomRight);

    RSL_GETPROP_DECL(normalized);
    RSL_GETPROP_DECL(transposed);

    RSL_GETPROP_DECL(topLeft);
    RSL_PUTPROP_DECL(topLeft);
    //RSL_METHOD_DECL(last);
    //RSL_METHOD_DECL(prev);
};

void RegisterRectRsl()
{
    QRectRsl::TablePtr->clsName = "QRect";
    _LibRslAddUniClass(QRectRsl::TablePtr, true);
}

Qt::HANDLE CreateRectRsl(const QRect &rect)
{
    QRectRsl *pRsl = nullptr;
    TGenClass *cls = (TGenClass*)_LibRslFindUniClass(QRectRsl::TablePtr->clsName);
    TGenObject *obj = (TGenObject*)_LibCreateObjectOfClassEx(cls, NULL, (void**)&pRsl);
    pRsl->rect = rect;

    return obj;
}

bool IsRectRsl(Qt::HANDLE obj)
{
    return _LibRslGetUniClass(obj) == _LibRslFindUniClass("QRect");
}

QRect GetRectRsl(Qt::HANDLE obj)
{
    QRectRsl *_rsl = nullptr;
    _LibRslUniCast("QRect", obj, (void**)&_rsl);

    if (!_rsl)
        return QRect();

    return _rsl->rect;
}

RSL_CLASS_BEGIN(QRectRsl)
RSL_INIT
RSL_PROP_METH2(x)
RSL_PROP_METH2(y)
RSL_PROP_METH2(width)
RSL_PROP_METH2(height)
RSL_PROP_METH2(size)
RSL_PROP_METH(isEmpty)
RSL_PROP_METH(isNull)
RSL_PROP_METH(isValid)
RSL_PROP_METH2(bottom)
RSL_PROP_METH2(left)
RSL_PROP_METH2(top)
RSL_PROP_METH2(right)
RSL_PROP_METH(center)
RSL_PROP_METH2(bottomLeft)
RSL_PROP_METH2(topRight)
RSL_PROP_METH2(bottomRight)
RSL_PROP_METH(normalized)
RSL_PROP_METH(transposed)
RSL_PROP_METH2(topLeft)

RSL_METH(contains)
RSL_METH(intersected)
RSL_METH(intersects)
RSL_METH(moveBottom)
RSL_METH(moveBottomLeft)
RSL_METH(moveBottomRight)
RSL_METH(moveCenter)
RSL_METH(moveLeft)
RSL_METH(moveRight)
RSL_METH(moveTo)
RSL_METH(moveTop)
RSL_METH(moveTopLeft)
RSL_METH(moveTopRight)
RSL_METH(translate)
RSL_METH(translated)
RSL_METH(united)
RSL_CLASS_END;

void QRectRsl::Initialise(int firstParmOffs)
{
    enum
    {
        prm_x = 0,
        prm_y,
        prm_width,
        prm_height
    };

    int count = GetFuncParamCount();
    if (count <= 2)
        return;

    if (count == 5)
    {
        if (GetFuncParamType(firstParmOffs + prm_x) != QVariant::Int)
            ThrowParamTypeError<int>(prm_x + firstParmOffs);

        if (GetFuncParamType(firstParmOffs + prm_y) != QVariant::Int)
            ThrowParamTypeError<int>(prm_y + firstParmOffs);

        if (GetFuncParamType(firstParmOffs + prm_width) != QVariant::Int)
            ThrowParamTypeError<int>(prm_width + firstParmOffs);

        if (GetFuncParamType(firstParmOffs + prm_height) != QVariant::Int)
            ThrowParamTypeError<int>(prm_height + firstParmOffs);

        int x = GetFuncParam(firstParmOffs + prm_x).toInt();
        int y = GetFuncParam(firstParmOffs + prm_y).toInt();
        int width = GetFuncParam(firstParmOffs + prm_width).toInt();
        int height = GetFuncParam(firstParmOffs + prm_height).toInt();

        rect = QRect(x, y, width, height);
    }
    else if (count == 3)
    {
        int type = GetFuncParamType(firstParmOffs + prm_x);
        int type2 = GetFuncParamType(firstParmOffs + prm_y);

        if (type == QVariant::Point && type2 == QVariant::Point)
        {
            QPoint topLeft = GetFuncParam(firstParmOffs + prm_x).toPoint();
            QPoint bottomRight = GetFuncParam(firstParmOffs + prm_y).toPoint();

            rect = QRect(topLeft, bottomRight);
        }
        else if (type == QVariant::Point && type2 == QVariant::Size)
        {
            QPoint topLeft = GetFuncParam(firstParmOffs + prm_x).toPoint();
            QSize size = GetFuncParam(firstParmOffs + prm_y).toSize();

            rect = QRect(topLeft, size);
        }
        else
            ThrowParamTypeError(firstParmOffs);
    }
}

RSL_INIT_IMP(QRectRsl)
{
    Initialise(*firstParmOffs);
}

RSL_GETPROP_IMP(QRectRsl, x)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.x());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, x)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.setX(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, y)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.y());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, y)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.setY(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, width)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.width());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, width)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.setWidth(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, height)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.height());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, height)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.setHeight(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, size)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.size());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, size)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::QSize)
        ThrowParamTypeError<QSize>(1);

    rect.setSize(GetFuncParam(1).toSize());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, isEmpty)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.isEmpty());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, isNull)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.isNull());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, isValid)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.isValid());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, bottom)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.bottom());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, bottom)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    rect.setBottom(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, left)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.left());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, left)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    rect.setLeft(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, top)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.top());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, top)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    rect.setTop(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, right)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.right());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, right)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    rect.setRight(GetFuncParam(1).toInt());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, center)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.center());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, bottomLeft)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.bottomLeft());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, bottomLeft)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::QPoint)
        ThrowParamTypeError<QPoint>(1);

    rect.setBottomLeft(GetFuncParam(1).toPoint());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, topRight)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.topRight());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, topRight)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::QPoint)
        ThrowParamTypeError<QPoint>(1);

    rect.setTopRight(GetFuncParam(1).toPoint());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, bottomRight)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.bottomRight());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, bottomRight)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::QPoint)
        ThrowParamTypeError<QPoint>(1);

    rect.setBottomRight(GetFuncParam(1).toPoint());

    return 0;
}

RSL_GETPROP_IMP(QRectRsl, normalized)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.normalized());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, transposed)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.transposed());
    return 0;
}

RSL_GETPROP_IMP(QRectRsl, topLeft)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rect.topLeft());
    return 0;
}

RSL_PUTPROP_IMP(QRectRsl, topLeft)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::QPoint)
        ThrowParamTypeError<QPoint>(1);

    rect.setTopLeft(GetFuncParam(1).toPoint());

    return 0;
}

RSL_METHOD_IMP(QRectRsl, contains)
{
    int type = GetFuncParamType(1);
    if (type == QVariant::Int)
    {
        int x = 0, y = 0;
        bool proper = false;

        x = GetFuncParam(1).toInt();

        type = GetFuncParamType(2);
        if (type != QVariant::Int)
            ThrowParamTypeError<int>(2);

        y = GetFuncParam(2).toInt();

        type = GetFuncParamType(3);
        if (type == QVariant::Bool)
            proper = GetFuncParam(3).toBool();

        bool ret = rect.contains(x, y, proper);

        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        ret);
    }
    else if (type == QVariant::Rect)
    {
        bool proper = false;
        QRect r = GetFuncParam(1).toRect();

        type = GetFuncParamType(2);
        if (type == QVariant::Bool)
            proper = GetFuncParam(2).toBool();

        bool ret = rect.contains(r);
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        ret);
    }
    else if (type == QVariant::Point)
    {
        bool proper = false;
        QPoint p = GetFuncParam(1).toPoint();

        type = GetFuncParamType(2);
        if (type == QVariant::Bool)
            proper = GetFuncParam(2).toBool();

        bool ret = rect.contains(p);
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        ret);
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, intersected)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Rect)
        ThrowParamTypeError<QRect>(1);

    QRect r = GetFuncParam(1).toRect();
    QRect ret = rect.intersected(r);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        ret);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, intersects)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Rect)
        ThrowParamTypeError<QRect>(1);

    QRect r = GetFuncParam(1).toRect();
    bool ret = rect.intersects(r);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        ret);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveBottom)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.moveBottom(GetFuncParam(1).toInt());

    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveBottomLeft)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Point)
        ThrowParamTypeError<QPoint>(1);

    QPoint p = GetFuncParam(1).toPoint();
    rect.moveBottomLeft(p);
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveBottomRight)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Point)
        ThrowParamTypeError<QPoint>(1);

    QPoint p = GetFuncParam(1).toPoint();
    rect.moveBottomRight(p);
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveCenter)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Point)
        ThrowParamTypeError<QPoint>(1);

    QPoint p = GetFuncParam(1).toPoint();
    rect.moveCenter(p);
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveLeft)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.moveLeft(GetFuncParam(1).toInt());
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveRight)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.moveRight(GetFuncParam(1).toInt());
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveTo)
{
    int type = GetFuncParamType(1);
    if (type == QVariant::Int)
    {
        int x = 0, y = 0;
        x = GetFuncParam(1).toInt();

        type = GetFuncParamType(2);
        if (type != QVariant::Int)
            ThrowParamTypeError<int>(2);

        y = GetFuncParam(2).toInt();
        rect.moveTo(x, y);
    }
    else if (type == QVariant::Point)
    {
        QPoint p = GetFuncParam(1).toPoint();
        rect.moveTo(p);
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveTop)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Int)
        ThrowParamTypeError<int>(1);

    rect.moveTop(GetFuncParam(1).toInt());
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveTopLeft)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Point)
        ThrowParamTypeError<QPoint>(1);

    QPoint p = GetFuncParam(1).toPoint();
    rect.moveTopLeft(p);
    return 0;
}

RSL_METHOD_IMP(QRectRsl, moveTopRight)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Point)
        ThrowParamTypeError<QPoint>(1);

    QPoint p = GetFuncParam(1).toPoint();
    rect.moveTopRight(p);
    return 0;
}

RSL_METHOD_IMP(QRectRsl, translate)
{
    int type = GetFuncParamType(1);
    if (type == QVariant::Int)
    {
        int x = 0, y = 0;
        x = GetFuncParam(1).toInt();

        type = GetFuncParamType(2);
        if (type != QVariant::Int)
            ThrowParamTypeError<int>(2);

        y = GetFuncParam(2).toInt();
        rect.translate(x, y);
    }
    else if (type == QVariant::Point)
    {
        bool proper = false;
        QPoint p = GetFuncParam(1).toPoint();
        rect.translate(p);
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, translated)
{
    int type = GetFuncParamType(1);
    if (type == QVariant::Int)
    {
        int x = 0, y = 0;
        x = GetFuncParam(1).toInt();

        type = GetFuncParamType(2);
        if (type != QVariant::Int)
            ThrowParamTypeError<int>(2);

        y = GetFuncParam(2).toInt();
        QRect rc = rect.translated(x, y);

        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rc);
    }
    else if (type == QVariant::Point)
    {
        QPoint p = GetFuncParam(1).toPoint();
        QRect rc = rect.translated(p);

        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rc);
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QRectRsl, united)
{
    int type = GetFuncParamType(1);
    if (type != QVariant::Rect)
        ThrowParamTypeError<QRect>(1);

    QRect r = GetFuncParam(1).toRect();
    QRect rc = rect.united(r);
    
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        rc);

    return 0;
}
