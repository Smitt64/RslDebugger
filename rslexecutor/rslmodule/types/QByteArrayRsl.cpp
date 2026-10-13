#include "rsl/dlmintf.h"
#include "statvars.h"
#include "rsl/isymbol.h"
#include "rsscript/registerobjlist.hpp"
#include "rslexecutor.h"
#include "rsscript/rslibdynamicfuncs.h"
#include <QByteArray>

class QByteArrayRsl
{
public:
    QByteArrayRsl(TGenObject *pThis = NULL)
    {
    }

    ~QByteArrayRsl()
    {

    }

    void Initialise(int firstParmOffs);
    QScopedPointer<QByteArray> byte;

    RSL_CLASS(QByteArrayRsl)
    RSL_INIT_DECL();
    RSL_GETPROP_DECL(size);
    RSL_GETPROP_DECL(capacity);
    RSL_GETPROP_DECL(data);
    RSL_GETPROP_DECL(isEmpty);
    RSL_GETPROP_DECL(isLower);
    RSL_GETPROP_DECL(isUpper);
    RSL_GETPROP_DECL(isNull);

    RSL_METHOD_DECL(append);
    RSL_METHOD_DECL(at);
    RSL_METHOD_DECL(codeAt);
    RSL_METHOD_DECL(chop);
    RSL_METHOD_DECL(clear);
    RSL_METHOD_DECL(compare);
    RSL_METHOD_DECL(contains);
    RSL_METHOD_DECL(count);
    RSL_METHOD_DECL(indexOf);
    RSL_METHOD_DECL(lastIndexOf);
    RSL_METHOD_DECL(left);
    RSL_METHOD_DECL(leftJustified);
    RSL_METHOD_DECL(mid);
    RSL_METHOD_DECL(insert);
    RSL_METHOD_DECL(prepend);
    RSL_METHOD_DECL(repeated);
    RSL_METHOD_DECL(replace);
    RSL_METHOD_DECL(reserve);
    RSL_METHOD_DECL(resize);
    RSL_METHOD_DECL(right);
    RSL_METHOD_DECL(rightJustified);
    RSL_METHOD_DECL(setNum);
    RSL_METHOD_DECL(startsWith);
    RSL_METHOD_DECL(toBase64);
    RSL_METHOD_DECL(toHex);
    RSL_METHOD_DECL(toLower);
    RSL_METHOD_DECL(toUpper);
    RSL_METHOD_DECL(trimmed);
    RSL_METHOD_DECL(truncate);
};

void RegisterByteArrayRsl()
{
    QByteArrayRsl::TablePtr->clsName = "QByteArray";
    _LibRslAddUniClass(QByteArrayRsl::TablePtr, true);
}

Qt::HANDLE CreateByteArrayRsl(const QByteArray &pt)
{
    QByteArrayRsl *pRsl = nullptr;
    TGenClass *cls = (TGenClass*)_LibRslFindUniClass(QByteArrayRsl::TablePtr->clsName);
    TGenObject *obj = (TGenObject*)_LibCreateObjectOfClassEx(cls, NULL, (void**)&pRsl);
    pRsl->byte.reset(new QByteArray(pt));

    return obj;
}

bool IsByteArrayRsl(Qt::HANDLE obj)
{
    return _LibRslGetUniClass(obj) == _LibRslFindUniClass("QByteArray");
}

QByteArray GetByteArrayRsl(Qt::HANDLE obj)
{
    QByteArrayRsl *_rsl = nullptr;
    _LibRslUniCast("QByteArray", obj, (void**)&_rsl);

    if (!_rsl)
        return QByteArray();

    return *_rsl->byte;
}

RSL_CLASS_BEGIN(QByteArrayRsl)
RSL_INIT
RSL_PROP_METH(size)
RSL_PROP_METH(capacity)
RSL_PROP_METH(data)
RSL_PROP_METH(isEmpty)
RSL_PROP_METH(isLower)
RSL_PROP_METH(isUpper)
RSL_PROP_METH(isNull)

RSL_METH(append)
RSL_METH(at)
RSL_METH(codeAt)
RSL_METH(chop)
RSL_METH(clear)
RSL_METH(compare)
RSL_METH(contains)
RSL_METH(count)
RSL_METH(indexOf)
RSL_METH(lastIndexOf)
RSL_METH(left)
RSL_METH(leftJustified)
RSL_METH(mid)
RSL_METH(insert)
RSL_METH(prepend)
RSL_METH(repeated)
RSL_METH(replace)
RSL_METH(reserve)
RSL_METH(resize)
RSL_METH(right)
RSL_METH(rightJustified)
RSL_METH(setNum)
RSL_METH(startsWith)
RSL_METH(toBase64)
RSL_METH(toHex)
RSL_METH(toLower)
RSL_METH(toUpper)
RSL_METH(trimmed)
RSL_METH(truncate)
RSL_CLASS_END;

void QByteArrayRsl::Initialise(int firstParmOffs)
{
    enum
    {
        prm_size = 0,
        prm_char
    };

    int ParamCount = GetFuncParamCount();
    if (ParamCount == 3)
    {
        int type = GetFuncParamType(prm_size + firstParmOffs);

        if (type == QVariant::Int)
        {
            int size = GetFuncParam(prm_size + firstParmOffs).toInt();
            
            type = GetFuncParamType(prm_char + firstParmOffs);
            if (type != QVariant::String)
                ThrowParamTypeError(prm_char + firstParmOffs);

            QString str = GetFuncParam(prm_char + firstParmOffs).toString();

            if (str.isEmpty())
                ThrowParamTypeError(prm_char + firstParmOffs);

            byte.reset(new QByteArray(size, str[0].toLatin1()));
        }
    }
    else if (ParamCount == 2)
    {
        int type = GetFuncParamType(prm_size + firstParmOffs);

        if (type == QVariant::String)
        {
            QString str = GetFuncParam(prm_char + firstParmOffs).toString();

            if (str.isEmpty())
                ThrowParamTypeError(prm_char + firstParmOffs);

            byte.reset(new QByteArray(str.toLocal8Bit().data()));
        }
        else
            ThrowParamTypeError(prm_size + firstParmOffs);
    }
    else
        byte.reset(new QByteArray());
}

RSL_INIT_IMP(QByteArrayRsl)
{
    Initialise(*firstParmOffs);
}

RSL_METHOD_IMP(QByteArrayRsl, append)
{
    enum
    {
        prm_str = 1,
        prm_chr = 0,
    };

    int ParamCount = GetFuncParamCount();
    if (ParamCount == 2)
    {
        int type = GetFuncParamType(prm_str);
        if (type == QVariant::String)
        {
            QString str = GetFuncParam(prm_str).toString();

            if (str.isEmpty())
                ThrowParamTypeError(prm_str);

            byte->append(str.toLocal8Bit().data());
        }
        else if (type == QVariant::ByteArray)
        {
            QByteArray str = GetFuncParam(prm_str).toByteArray();
            byte->append(str);
        }
    }
    else if (ParamCount == 3)
    {
        int type = GetFuncParamType(prm_str);

        if (type == QVariant::Int)
        {
            int size = GetFuncParam(prm_str ).toInt();
            
            if (type != QVariant::String)
                ThrowParamTypeError(prm_chr);

            QString str = GetFuncParam(prm_chr).toString();

            if (str.isEmpty())
                ThrowParamTypeError(prm_chr);

            byte->append(size, str[0].toLatin1());
        }
    }

    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, size)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->size());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, data)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->data());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, capacity)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->capacity());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, isEmpty)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->isEmpty());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, isLower)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->isLower());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, isUpper)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->isUpper());
    return 0;
}

RSL_GETPROP_IMP(QByteArrayRsl, isNull)
{
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->isNull());
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, at)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        QString(byte->at(i)));

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, codeAt)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        byte->at(i));

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, chop)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    byte->chop(i);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, clear)
{
    byte->clear();
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, compare)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    int result = -1;
    Qt::CaseSensitivity CaseSensitive = Qt::CaseSensitive;

    if (type2 == QMetaType::Bool)
    {
        bool b = GetFuncParam(2).toBool();
        if (b)
            CaseSensitive = Qt::CaseSensitive;
        else
            CaseSensitive = Qt::CaseInsensitive;
    }

    if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->compare(str.toLocal8Bit(), CaseSensitive);
    }
    else if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->compare(str, CaseSensitive);
    }
    else
        ThrowParamTypeError(1);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        result);
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, contains)
{
    int type = GetFuncParamType(1);

    bool result = false;
    if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->contains(str.toLocal8Bit());
    }
    else if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->contains(str);
    }
    else
        ThrowParamTypeError(1);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        result);
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, count)
{
    int type = GetFuncParamType(1);

    int result = false;
    if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->count(str.toLocal8Bit());
    }
    else if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->count(str);
    }
    else
        ThrowParamTypeError(1);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        result);
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, indexOf)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    int result = false;
    int from = 0;
    if (type2 == QMetaType::Int)
        from = GetFuncParam(2).toInt();

    if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->indexOf(str.toLocal8Bit(), from);
    }
    else if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->indexOf(str, from);
    }
    else
        ThrowParamTypeError(1);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        result);
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, lastIndexOf)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    int result = false;
    int from = 0;
    if (type2 == QMetaType::Int)
        from = GetFuncParam(2).toInt();

    if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->lastIndexOf(str.toLocal8Bit(), from);
    }
    else if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        result = byte->lastIndexOf(str, from);
    }
    else
        ThrowParamTypeError(1);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        result);
    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, left)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    QByteArray left = byte->left(i);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        left);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, leftJustified)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);
    int type3 = GetFuncParamType(3);

    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    bool truncate = false;
    int width = GetFuncParam(1).toInt();
    char fill = ' ';

    if (type2 == QMetaType::QString)
    {
        QString str = GetFuncParam(2).toString();

        if (str.isEmpty())
            ThrowParamTypeError(2);

        fill = str.toLocal8Bit().at(0);
    }

    if (type3 == QMetaType::Bool)
        truncate = GetFuncParam(3).toBool();
    
    QByteArray left = byte->leftJustified(width, fill, truncate);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        left);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, mid)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int len = -1;
    if (type == QMetaType::Int)
        len = GetFuncParam(2).toInt();

    int i = GetFuncParam(1).toInt();
    QByteArray mid = byte->mid(i, len);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        mid);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, insert)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    if (type2 == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(2).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(2);

        QByteArray result = byte->insert(i, str);
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                      std::placeholders::_1,
                                      std::placeholders::_2),
                            result);
    }
    else if (type2 == QMetaType::QString)
    {
        QString str = GetFuncParam(2).toString();

        if (str.isEmpty())
            ThrowParamTypeError(2);

        QByteArray result = byte->insert(i, str.toLocal8Bit());
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                      std::placeholders::_1,
                                      std::placeholders::_2),
                            result);
    }
    else if (type2 == QMetaType::Int)
    {
        int c = GetFuncParam(2).toInt();

        QByteArray result = byte->insert(i, c);
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                      std::placeholders::_1,
                                      std::placeholders::_2),
                            result);
    }

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, prepend)
{
    int type = GetFuncParamType(1);

    if (type == QMetaType::QByteArray)
    {
        QByteArray str = GetFuncParam(1).toByteArray();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        byte->prepend(str);
    }
    else if (type == QMetaType::QString)
    {
        QString str = GetFuncParam(1).toString();

        if (str.isEmpty())
            ThrowParamTypeError(1);

        byte->prepend(str.toLocal8Bit());
    }
    else if (type == QMetaType::Int)
    {
        int c = GetFuncParam(1).toInt();
        byte->prepend((char)c);
    }

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, repeated)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    QByteArray repeated = byte->repeated(i);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        repeated);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, replace)
{
    int paramCount = GetFuncParamCount();
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);

    if (type == QMetaType::Int)
    {
        int type3 = GetFuncParamType(3);

        if (paramCount == 4)
        {
            if (type2 != QMetaType::Int)
                ThrowParamTypeError<int>(1);

            int len = GetFuncParam(2).toInt();
            if (type3 == QMetaType::QByteArray)
            {
                QByteArray str2 = GetFuncParam(3).toByteArray();

                QByteArray result = byte->replace(GetFuncParam(1).toInt(), len, str2);
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                            std::placeholders::_1,
                                            std::placeholders::_2),
                                    result);
            }
            else if (type3 == QMetaType::QString)
            {
                QString str2 = GetFuncParam(3).toString();

                QByteArray result = byte->replace(GetFuncParam(1).toInt(), len, str2.toLocal8Bit());
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                            std::placeholders::_1,
                                            std::placeholders::_2),
                                    result);
            }
        }
        else
            ThrowParamTypeError(1);
    }
    else if (paramCount == 3)
    {
        if (type == QMetaType::QString)
        {
            if (type2 == QMetaType::QString)
            {
                QString str2 = GetFuncParam(1).toString();
                QString str3 = GetFuncParam(2).toString();

                QByteArray result = byte->replace(str2.toLocal8Bit(), str3.toLocal8Bit());
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    result);
            }
            else if (type2 == QMetaType::QByteArray)
            {
                QString str2 = GetFuncParam(1).toString();
                QByteArray str3 = GetFuncParam(2).toByteArray();

                QByteArray result = byte->replace(str2.toLocal8Bit(), str3);
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    result);
            }
        }
        else if (type == QMetaType::QByteArray)
        {
            if (type2 == QMetaType::QString)
            {
                QByteArray str2 = GetFuncParam(1).toByteArray();
                QString str3 = GetFuncParam(2).toString();

                QByteArray result = byte->replace(str2, str3.toLocal8Bit());
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    result);
            }
            else if (type2 == QMetaType::QByteArray)
            {
                QByteArray str2 = GetFuncParam(1).toByteArray();
                QByteArray str3 = GetFuncParam(2).toByteArray();

                QByteArray result = byte->replace(str2, str3);
                SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    result);
            }
        }
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, reserve)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    byte->reserve(i);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, resize)
{
    int type = GetFuncParamType(1);
    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    byte->resize(i);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, right)
{
    int type = GetFuncParamType(1);

    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    int i = GetFuncParam(1).toInt();
    QByteArray result = byte->right(i);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                              std::placeholders::_1,
                                              std::placeholders::_2),
                                    result);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, rightJustified)
{
    int type = GetFuncParamType(1);
    int type2 = GetFuncParamType(2);
    int type3 = GetFuncParamType(3);

    if (type != QMetaType::Int)
        ThrowParamTypeError<int>(1);

    bool truncate = false;
    int width = GetFuncParam(1).toInt();
    char fill = ' ';

    if (type2 == QMetaType::QString)
    {
        QString str = GetFuncParam(2).toString();

        if (str.isEmpty())
            ThrowParamTypeError(2);

        fill = str.toLocal8Bit().at(0);
    }

    if (type3 == QMetaType::Bool)
        truncate = GetFuncParam(3).toBool();
    
    QByteArray left = byte->rightJustified(width, fill, truncate);

    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                  std::placeholders::_1,
                                  std::placeholders::_2),
                        left);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, setNum)
{
    int type = GetFuncParamType(1);

    if (type == QMetaType::Int)
    {
        int base = 10;
        int type2 = GetFuncParamType(2);

        if (type2 == QMetaType::Int)
            base = GetFuncParam(2).toInt();

        int i = GetFuncParam(1).toInt();
        byte->setNum(i, base);
    }
    else if (type == QMetaType::Float || type == QMetaType::Double)
    {
        char f = 'g';
        int prec = 6;

        int type2 = GetFuncParamType(2);
        int type3 = GetFuncParamType(3);

        if (type2 == QMetaType::QString)
            f = GetFuncParam(2).toString().front().toLatin1();

        if (type3 == QMetaType::Int)
            prec = GetFuncParam(3).toInt();

        if (type == QMetaType::Float)
        {
            float i = GetFuncParam(1).toFloat();
            byte->setNum(i, f, prec);
        }
        else if (type == QMetaType::Double)
        {
            double i = GetFuncParam(1).toDouble();
            byte->setNum(i, f, prec);
        }
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, startsWith)
{
    int type = GetFuncParamType(1);

    if (type == QMetaType::QByteArray)
    {
        QByteArray i = GetFuncParam(1).toByteArray();
        bool result = byte->startsWith(i);

        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                                std::placeholders::_1,
                                                std::placeholders::_2),
                                        result);
    }
    else if (type == QMetaType::QString)
    {
        QString i = GetFuncParam(1).toString();
        bool result = byte->startsWith(i.toLocal8Bit());

        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                                                std::placeholders::_1,
                                                std::placeholders::_2),
                                        result);
    }
    else
        ThrowParamTypeError(1);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, toBase64)
{
    int ParamCount = GetFuncParamCount();

    if (ParamCount == 1)
    {
        QByteArray result = byte->toBase64();
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
            std::placeholders::_1, std::placeholders::_2), result);
    }
    else
    {
        int type = GetFuncParamType(1);

        if (type == QMetaType::Int)
        {
            int i = GetFuncParam(1).toInt();
            QByteArray result = byte->toBase64((QByteArray::Base64Options)i);

            SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                std::placeholders::_1, std::placeholders::_2), result);
        }
        else
            ThrowParamTypeError<int>(1);
    }

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, toHex)
{
    int ParamCount = GetFuncParamCount();

    if (ParamCount == 1)
    {
        QByteArray result = byte->toHex();
        SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
            std::placeholders::_1, std::placeholders::_2), result);
    }
    else
    {
        int type = GetFuncParamType(1);

        if (type == QMetaType::QString)
        {
            QString sep = GetFuncParam(1).toString();
            QByteArray result = byte->toHex(sep.front().toLatin1());

            SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
                std::placeholders::_1, std::placeholders::_2), result);
        }
        else
            ThrowParamTypeError<QString>(1);
    }

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, toLower)
{
    QByteArray result = byte->toLower();
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
        std::placeholders::_1, std::placeholders::_2), result);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, toUpper)
{
    QByteArray result = byte->toUpper();
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
        std::placeholders::_1, std::placeholders::_2), result);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, trimmed)
{
    QByteArray result = byte->trimmed();
    SetValueFromVariant(std::bind(StdValueSetFunc, retVal,
        std::placeholders::_1, std::placeholders::_2), result);

    return 0;
}

RSL_METHOD_IMP(QByteArrayRsl, truncate)
{
    int type = GetFuncParamType(1);

    if (type == QMetaType::Int)
    {
        int i = GetFuncParam(1).toInt();
        byte->truncate(i);
    }
    else
        ThrowParamTypeError<int>(1);

    return 0;
}
