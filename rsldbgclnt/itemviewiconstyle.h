#ifndef ITEMVIEWICONSTYLE_H
#define ITEMVIEWICONSTYLE_H

#include <QProxyStyle>
#include <QStyleOptionViewItem>

// Прокси-стиль, исправляющий растягивание SVG-иконок темы в item views.
//
// Стиль windowsvista в subElementRect(SE_ItemViewItemDecoration) намеренно
// расширяет прямоугольник иконки на 2 пикселя с каждой стороны
// (qwindowsvistastyle.cpp: rect.adjust(-2, 0, 2, 0)) — это декоративные поля
// вокруг растровых иконок: их actualSize() возвращает нативный размер, поэтому
// иконка просто центрируется и визуально ничего не меняется.
//
// Но для Scalable-темы QIconLoaderEngine::actualSize() возвращает запрошенный
// размер как есть, поэтому QIcon::paint получает прямоугольник 20x16 вместо
// 16x16, а квадратный SVG-pixmap растягивается по горизонтали (x1.25).
//
// Здесь расширение отменяется: для растровых иконок это по-прежнему ничего
// не меняет (они центрируются в прямоугольнике), а для SVG возвращает
// квадратный прямоугольник и корректные пропорции.
class ItemViewIconStyle : public QProxyStyle
{
public:
    explicit ItemViewIconStyle(QStyle *baseStyle = Q_NULLPTR)
        : QProxyStyle(baseStyle)
    {
    }

    QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = Q_NULLPTR) const Q_DECL_OVERRIDE
    {
        QRect rect = QProxyStyle::subElementRect(element, option, widget);

        if (element == SE_ItemViewItemDecoration
            && qstyleoption_cast<const QStyleOptionViewItem *>(option)
            && baseStyle() && baseStyle()->objectName() == QLatin1String("windowsvista"))
        {
            rect.adjust(2, 0, -2, 0);
        }

        return rect;
    }
};

#endif // ITEMVIEWICONSTYLE_H
