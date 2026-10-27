#ifndef WINDOWTITLEBAR_H
#define WINDOWTITLEBAR_H

#include <QMenuBar>

// Меню-бар, встроенный в заголовок окна (в стиле Visual Studio).
// Дополнительно рисует иконку приложения слева и заголовок окна по центру.
class WindowTitleMenuBar : public QMenuBar
{
    Q_OBJECT

public:
    explicit WindowTitleMenuBar(QWidget *parent = nullptr);

    // Ширина области справа, занятой системными кнопками:
    // в ней центральный заголовок не рисуется.
    void setRightReserved(int w);
    int rightReserved() const;

    // Прямоугольники пунктов меню в координатах окна.
    // Используются hit-test'ом заголовка: по этим областям
    // нельзя возвращать HTCAPTION, иначе меню не будут кликаться.
    QList<QRect> interactiveRects() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;

private:
    int m_RightReserved = 0;
};

#endif // WINDOWTITLEBAR_H
