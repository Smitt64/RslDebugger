// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#ifndef HIGHLIGHTERSTYLE_H_
#define HIGHLIGHTERSTYLE_H_

#include "ToolsRuntime_global.h"
#include <QTextCharFormat>
#include <QLoggingCategory>

#define FormatKeyword "keyword"
#define FormatKeyword2 "keyword2"
#define FormatComment "comment"
#define FormatDefault "plain"
#define FormatFunction "function"
#define FormatNumber "number"
#define FormatStrig "string"
#define FormatEnum "enum"
#define FormatDefine "define"
#define FormatElemType "type"

#define SYNTAXHIGHLIGHTER_FOLDER "syntaxhighlighter"
TOOLSRUNTIME_EXPORT Q_DECLARE_LOGGING_CATEGORY(logHighlighter);

class QColor;
class HighlighterStylePrivate;
class StyleItemPrivate;
class QTextCharFormat;
class TOOLSRUNTIME_EXPORT StyleItem
{
public:
    StyleItem();
    void load(const QString &filename);
    void save(const QString &filename = QString());

    QTextCharFormat &format(const QString &alias);
    bool hasFormat(const QString &alias) const;

    const QColor &editorBackground() const;
    const QColor &editorCurrentLine() const;
    const QColor &editorCurrentWord() const;

    const QColor &linenumbersBackground() const;
    const QColor &linenumbersForeground() const;

    void setEditorBackground(const QColor &color);
    void setEditorCurrentLine(const QColor &color);
    void setEditorCurrentWord(const QColor &color);

    void setLinenumbersBackground(const QColor &color);
    void setLinenumbersForeground(const QColor &color);

    const QString &filename() const;

private:
    StyleItemPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(StyleItem);
};

class TOOLSRUNTIME_EXPORT HighlighterStyle
{
public:
    virtual ~HighlighterStyle();

    static HighlighterStyle *inst();
    QSharedPointer<StyleItem> style(const QString &name = QString()) const;
    QStringList themes() const;

    void setDefaultTheme(const QString &theme);
    QString defaultTheme() const;
    void loadStyle(const QString &filename);

    bool isStatic(const QString &theme) const;
    void deleteStyle(const QString &theme);

private:
    HighlighterStyle();
    static HighlighterStyle *m_Inst;

    HighlighterStylePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(HighlighterStyle);
};

#endif // HIGHLIGHTERSTYLE_H
