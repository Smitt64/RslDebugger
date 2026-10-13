// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include "ToolsRuntime_global.h"

class CodeHighlighter;
class CodeEditorPrivate;
class CodeEditorLineWidgetProvider;
class TOOLSRUNTIME_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
    friend class LineNumberArea;
    friend class CodeEditorOptionsPage;
    friend TOOLSRUNTIME_EXPORT void ToolApplyHighlighter(QPlainTextEdit *pEditor,
                                     const int &syntax,
                                     const QString &style);
public:
    CodeEditor(QWidget *parent = nullptr);
    virtual ~CodeEditor();

    void setTabStop(int w);
    void setCurrentLineColor(const QColor &color);
    void setCurrentWordColor(const QColor &color);

    CodeHighlighter *highlighter();
    void setStyle(const QString &style);
    void rehighlight();

    void setCodeEditorLineWidgetProvider(CodeEditorLineWidgetProvider *provider);
    CodeEditorLineWidgetProvider *codeEditorLineWidgetProvider();

    void setAutoHighlightCurrentLine(const bool &v);
    bool autoHighlightCurrentLine() const;

    void setCurrentHighlightLine(const int &line);
    void appendUserSelection(const QTextCursor &cursor, const QTextCharFormat &format);
    void clearUserSelection();

protected:
    void setCodeHighlighter(CodeHighlighter *highlighter);
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

signals:
    void currentLineChanged(const int &last, const int &cur);
    void lineNumberClicked(const int &line);

private:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    CodeEditorPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(CodeEditor);
};

class CodeEditorLineWidgetProvider
{
public:
    virtual void addItemId(const int &id, const int &line) {};
    virtual void removeItemId(const int &id, const int &line) {};

    virtual int width() const = 0;
    virtual void paint(QPainter *painter, const int &line, const QRect &rc) = 0;
    virtual void lineClick(const int &line, const Qt::MouseButton &button) {};
};

#endif // CODEEDITOR_H
