// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "codeeditor.h"
#include "codeeditor/codehighlighter.h"
#include "codeeditor/highlighterstyle.h"
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>
#include <QStyle>
#include <QStyleOption>
#include <QApplication>
#include <QDebug>

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor)
    {
        codeEditor = editor;
    }

    ~LineNumberArea()
    {
        codeEditor = NULL;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE
    {
        m_MousePressPoint = event->pos();
    }

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE
    {
        CodeEditorLineWidgetProvider *provider = codeEditor->codeEditorLineWidgetProvider();

        if (!provider)
            return;

        QTextBlock block = codeEditor->firstVisibleBlock();
        int blockNumber = block.blockNumber();

        while (block.isValid())
        {
            QRectF rect = codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset());
            rect.setX(0);
            rect.setWidth(provider->width());
            QPointF translated(m_MousePressPoint.x(), m_MousePressPoint.y());

            if (rect.contains(translated))
            {
                if (provider)
                {
                    provider->lineClick(blockNumber + 1, event->button());
                    update();
                    emit codeEditor->lineNumberClicked(blockNumber + 1);
                }
            }

            block = block.next();
            blockNumber ++;
        }
    }

private:
    CodeEditor *codeEditor;
    QPoint m_MousePressPoint;
};

class CodeEditorPrivate
{
    Q_DECLARE_PUBLIC(CodeEditor)
public:
    CodeEditorPrivate(CodeEditor *obj)
    {
        q_ptr = obj;
        currentHighlightLine = 0;
        m_AutoHighlightCurrentLine = true;
        m_CurrentWordColor = QColor("#9BFF9B");

        m_CurrentLineColor = QColor(Qt::yellow).lighter(160);
        lineNumberArea = new LineNumberArea(obj);

        m_pCodeHighlighter = nullptr;
        m_pIconProvider = nullptr;
    }

    void lineNumberAreaPaintEvent(QPaintEvent *event)
    {
        Q_Q(CodeEditor);
        QPainter painter(lineNumberArea);

        QStyleOption opt;
        opt.init(lineNumberArea);
        opt.rect = event->rect();
        QStyle *s = qApp->style();
        int line_offset = 0;
        QPalette palette = s->standardPalette();
        QColor linenumbersBackground = palette.color(QPalette::Normal, QPalette::Window);
        QColor linenumbersForeground = palette.color(QPalette::Normal, QPalette::Text);
        QColor editorBackground = palette.color(QPalette::Normal, QPalette::Light);
        QColor editorCurrentLine = palette.color(QPalette::Normal, QPalette::Light);

        if (m_pCodeHighlighter && m_pCodeHighlighter->style())
        {
            if (m_pCodeHighlighter->style()->linenumbersBackground().isValid())
                linenumbersBackground = m_pCodeHighlighter->style()->linenumbersBackground();

            if (m_pCodeHighlighter->style()->linenumbersForeground().isValid())
                linenumbersForeground = m_pCodeHighlighter->style()->linenumbersForeground();

            if (m_pCodeHighlighter->style()->editorBackground().isValid())
                editorBackground = m_pCodeHighlighter->style()->editorBackground();

            if (m_pCodeHighlighter->style()->editorCurrentLine().isValid())
                editorCurrentLine = m_pCodeHighlighter->style()->editorCurrentLine();
        }

        painter.fillRect(event->rect(), editorBackground);

        if (m_pIconProvider)
        {
            line_offset = m_pIconProvider->width();

            QRect rc = event->rect();
            rc.setWidth(line_offset);

            painter.save();
            painter.fillRect(rc, linenumbersBackground);
            painter.restore();
        }

        QLine l(QPoint(opt.rect.width() - 1, opt.rect.top()), QPoint(opt.rect.width() - 1, opt.rect.bottom()));
        painter.save();
        painter.setPen(editorCurrentLine);
        painter.drawLine(l);
        painter.restore();

        QTextBlock block = q->firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = (int) q->blockBoundingGeometry(block).translated(q->contentOffset()).top();
        int bottom = top + (int) q->blockBoundingRect(block).height();

        while (block.isValid() && top <= event->rect().bottom())
        {
            if (block.isVisible() && bottom >= event->rect().top())
            {
                painter.save();
                QFont f = painter.font();
                f.setBold(false);
                QString number = QString::number(blockNumber + 1);
                painter.setPen(linenumbersForeground);

                if ((block == q->textCursor().block() && m_AutoHighlightCurrentLine) ||
                    (!m_AutoHighlightCurrentLine && block.blockNumber() + 1 == currentHighlightLine))
                {
                    painter.setPen(linenumbersForeground);
                    f.setBold(true);
                }

                painter.setFont(f);
                painter.drawText(line_offset, top, lineNumberArea->width() - 4 - line_offset, q->fontMetrics().height(),
                                 Qt::AlignRight, number);
                painter.restore();

                if (m_pIconProvider)
                {
                    painter.save();
                    m_pIconProvider->paint(&painter, blockNumber + 1, QRect(0, top, line_offset, q->blockBoundingGeometry(q->firstVisibleBlock()).height()));
                    painter.restore();
                }
            }

            block = block.next();
            top = bottom;
            bottom = top + (int) q->blockBoundingRect(block).height();
            ++blockNumber;
        }
    }

    void ApplyExtraSelections()
    {
        Q_Q(CodeEditor);
        QList<QTextEdit::ExtraSelection> selections;
        selections.append(selectionCurrentLine);
        selections.append(Selections);
        selections.append(userSelections);
        q->setExtraSelections(selections);
    }

    void onSelectionChanged()
    {
        Q_Q(CodeEditor);

        QTextCursor current = q->textCursor();

        if (lastLine != current.block().blockNumber())
        {
            emit q->currentLineChanged(lastLine, current.block().blockNumber());
            lastLine = current.block().blockNumber();
        }

        QString searchString = current.selectedText();
        searchString = searchString.simplified();
        Selections.clear();
        if (!searchString.isEmpty())
        {
            QBrush backBrush(m_CurrentWordColor);//"#9BFF9B"

            QTextCursor highlightCursor(q->document());
            while (!highlightCursor.isNull() && !highlightCursor.atEnd())
            {
                highlightCursor = q->document()->find(searchString, highlightCursor, QTextDocument::FindWholeWords);

                if (!highlightCursor.isNull())
                {
                    QTextEdit::ExtraSelection selection;
                    selection.cursor = highlightCursor;
                    selection.format.setBackground(backBrush);
                    Selections.append(selection);
                }
            }
        }

        ApplyExtraSelections();
    }

    void updateLineNumberArea(const QRect &rect, int dy)
    {
        Q_Q(CodeEditor);

        if (dy)
            lineNumberArea->scroll(0, dy);
        else
        {
            int w = lineNumberArea->width();

            if (m_pIconProvider)
                w += m_pIconProvider->width() + 2;

            lineNumberArea->update(0, rect.y(), w, rect.height());
        }

        if (rect.contains(q->viewport()->rect()))
            q->setViewportMargins(q->lineNumberAreaWidth(), 0, 0, 0);
    }

    QWidget *lineNumberArea;
    QTextEdit::ExtraSelection selectionCurrentLine;
    QList<QTextEdit::ExtraSelection> Selections, applyExtraSelections, userSelections;

    int tabStop, lastLine, currentHighlightLine;

    QColor m_CurrentLineColor, m_CurrentWordColor;

    bool m_AutoHighlightCurrentLine;
    CodeHighlighter *m_pCodeHighlighter;
    CodeEditorLineWidgetProvider *m_pIconProvider;
    CodeEditor *q_ptr;
};

CodeEditor::CodeEditor(QWidget *parent) :
    QPlainTextEdit(parent),
    d_ptr(new CodeEditorPrivate(this))
{
    Q_D(CodeEditor);
    QFont font("Courier New", 10);
    document()->setDefaultFont(font);

    connect(this, &CodeEditor::blockCountChanged, [=]()
    {
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    });

    connect(this, &CodeEditor::updateRequest, [=]()
    {
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    });

    connect(this, &CodeEditor::cursorPositionChanged, [=]()
    {
        if (d->m_AutoHighlightCurrentLine)
        {
            d->selectionCurrentLine.format.setBackground(d->m_CurrentLineColor);
            d->selectionCurrentLine.format.setProperty(QTextFormat::FullWidthSelection, true);
            d->selectionCurrentLine.cursor = textCursor();
            d->selectionCurrentLine.cursor.clearSelection();
        }

        d->ApplyExtraSelections();
    });

    connect(this, &CodeEditor::selectionChanged, [=]()
    {
        d->onSelectionChanged();
    });

    connect(this, &CodeEditor::updateRequest, [=](const QRect &rect, int dy)
    {
        d->updateLineNumberArea(rect, dy);
    });

    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

CodeEditor::~CodeEditor()
{
    Q_D(CodeEditor);

    if (d->m_pCodeHighlighter)
        delete d->m_pCodeHighlighter;

    delete d_ptr;
}

void CodeEditor::setAutoHighlightCurrentLine(const bool &v)
{
    Q_D(CodeEditor);
    d->m_AutoHighlightCurrentLine = v;
}

bool CodeEditor::autoHighlightCurrentLine() const
{
    Q_D(const CodeEditor);
    return d->m_AutoHighlightCurrentLine;
}

void CodeEditor::setCurrentHighlightLine(const int &line)
{
    Q_D(CodeEditor);
    d->currentHighlightLine = line;

    if (line > 0)
    {
        QTextDocument *doc = document();
        QTextBlock block = doc->findBlockByLineNumber(d->currentHighlightLine - 1);

        if (block.isValid())
        {
            d->selectionCurrentLine.format.setBackground(d->m_CurrentLineColor);
            d->selectionCurrentLine.format.setProperty(QTextFormat::FullWidthSelection, true);
            d->selectionCurrentLine.cursor = QTextCursor(block);
            d->selectionCurrentLine.cursor.clearSelection();

            setTextCursor(d->selectionCurrentLine.cursor);
        }
    }

    d->ApplyExtraSelections();
}

void CodeEditor::appendUserSelection(const QTextCursor &cursor, const QTextCharFormat &format)
{
    Q_D(CodeEditor);

    QTextEdit::ExtraSelection element;
    element.cursor = cursor;
    element.format = format;

    d->userSelections.append(element);
    d->ApplyExtraSelections();
}

void CodeEditor::clearUserSelection()
{
    Q_D(CodeEditor);
    d->userSelections.clear();
    d->ApplyExtraSelections();
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    Q_D(CodeEditor);
    d->lineNumberAreaPaintEvent(event);
}

int CodeEditor::lineNumberAreaWidth()
{
    Q_D(CodeEditor);

    int digits = 1;
    int max = qMax(4, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 1 + fontMetrics().width(QLatin1Char('9')) * /*digits*/5;

    if (d->m_pIconProvider)
        space += 2 + d->m_pIconProvider->width();

    return space;
}

void CodeEditor::setTabStop(int w)
{
    Q_D(CodeEditor);
    d->tabStop = w;
}

void CodeEditor::setCurrentLineColor(const QColor &color)
{
    Q_D(CodeEditor);
    d->m_CurrentLineColor = color;
}

void CodeEditor::setCurrentWordColor(const QColor &color)
{
    Q_D(CodeEditor);
    d->m_CurrentWordColor = color;
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    Q_D(CodeEditor);
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    d->lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::setCodeHighlighter(CodeHighlighter *highlighter)
{
    Q_D(CodeEditor);
    d->m_pCodeHighlighter = highlighter;
}

void CodeEditor::setCodeEditorLineWidgetProvider(CodeEditorLineWidgetProvider *provider)
{
    Q_D(CodeEditor);
    d->m_pIconProvider = provider;
    d->lineNumberArea->update();
    update();
}

CodeEditorLineWidgetProvider *CodeEditor::codeEditorLineWidgetProvider()
{
    Q_D(CodeEditor);
    return d->m_pIconProvider;
}

void CodeEditor::rehighlight()
{
    Q_D(CodeEditor);
    if (!d->m_pCodeHighlighter)
        return;

    QSharedPointer<StyleItem> sstyle = d->m_pCodeHighlighter->style();
    QTextCharFormat def =  sstyle->format(FormatDefault);
    QColor background = sstyle->editorBackground();

    QFont defaultFont = def.font();
    defaultFont.setStyleHint(QFont::Courier);
    document()->setDefaultFont(defaultFont);

    setStyleSheet(QString("QPlainTextEdit { background-color: rgb(%1, %2, %3); color: rgb(%4, %5, %6) }")
                               .arg(background.red())
                               .arg(background.green())
                               .arg(background.blue())
                               .arg(def.foreground().color().red())
                               .arg(def.foreground().color().green())
                               .arg(def.foreground().color().blue()));

    setCurrentLineColor(sstyle->editorCurrentLine());
    setCurrentWordColor(sstyle->editorCurrentWord());

    setAutoFillBackground(true);

    d->m_pCodeHighlighter->rehighlight();
    d->lineNumberArea->update();

    update();
}

CodeHighlighter *CodeEditor::highlighter()
{
    Q_D(CodeEditor);
    return d->m_pCodeHighlighter;
}

void CodeEditor::setStyle(const QString &style)
{
    Q_D(CodeEditor);

    if (d->m_pCodeHighlighter)
        d->m_pCodeHighlighter->setStyle(style);
}
