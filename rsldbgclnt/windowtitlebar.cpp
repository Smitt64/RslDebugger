#include "windowtitlebar.h"

#include <QAction>
#include <QEvent>
#include <QFontMetrics>
#include <QIcon>
#include <QPainter>
#include <QSizePolicy>

WindowTitleMenuBar::WindowTitleMenuBar(QWidget *parent)
    : QMenuBar(parent)
{
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void WindowTitleMenuBar::setRightReserved(int w)
{
    if (m_RightReserved == w)
        return;

    m_RightReserved = w;
    update();
}

int WindowTitleMenuBar::rightReserved() const
{
    return m_RightReserved;
}

QList<QRect> WindowTitleMenuBar::interactiveRects() const
{
    QList<QRect> rects;
    const QPoint offset = mapTo(window(), QPoint(0, 0));

    for (QAction *action : actions())
    {
        const QRect r = actionGeometry(action);

        if (!r.isNull())
            rects.append(r.translated(offset));
    }

    return rects;
}

void WindowTitleMenuBar::paintEvent(QPaintEvent *event)
{
    QMenuBar::paintEvent(event);

    QPainter p(this);

    // Иконка приложения слева (место зарезервировано padding-left в QSS)
    const QIcon icon = windowIcon();

    if (!icon.isNull())
        icon.paint(&p, QRect(7, (height() - 16) / 2, 16, 16));

    // Заголовок окна по центру свободной области между меню и системными кнопками
    int left = 28;

    for (QAction *action : actions())
    {
        const QRect r = actionGeometry(action);

        if (!r.isNull())
            left = qMax(left, r.right() + 8);
    }

    const int right = width() - m_RightReserved - 8;
    const QString title = window() ? window()->windowTitle() : QString();

    if (right - left > 40 && !title.isEmpty())
    {
        const QFontMetrics fm(font());
        p.setPen(palette().color(QPalette::WindowText));
        p.drawText(QRect(left, 0, right - left, height()), Qt::AlignCenter,
                   fm.elidedText(title, Qt::ElideRight, right - left));
    }
}

void WindowTitleMenuBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowTitleChange)
        update();

    QMenuBar::changeEvent(event);
}
