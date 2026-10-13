// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "codeeditor/highlighterstyle.h"
#include "codeeditor/codeeditor.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QTextCharFormat>
#include <QDir>
#include <QApplication>
#include <QMapIterator>

Q_LOGGING_CATEGORY(logHighlighter, "HighlighterStyle")
HighlighterStyle *HighlighterStyle::m_Inst = nullptr;

#define NO_ELEMENT_TEMPLATE QString("Element \"%1\" not exists")
#define HEX_COLOR_REGEX QString("#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{3})")

typedef struct FmtGenHighlightingRule
{
    QRegularExpression pattern;
    QString alias;
}FmtGenHighlightingRule;
typedef QList<FmtGenHighlightingRule> FmtGenHighlightingRuleList;

typedef QPair<qint32, qint32> Range;
typedef QList<Range> RangeList;

void CheckColorFormat(const QString &hex, const QString &field)
{
    QRegExp rx(HEX_COLOR_REGEX);

    if (!rx.exactMatch(hex))
        throw QString("Invalid \"%1\" value").arg(field);
}

class StyleItemPrivate
{
    Q_DECLARE_PUBLIC(StyleItem);
public:
    StyleItemPrivate(StyleItem *parent)
    {
        q_ptr = parent;
    }

    StyleItem *q_ptr;

    QString m_FileName;
    QMap<QString, QTextCharFormat> m_Formats;
    QColor m_EditorBackground, m_EditorCurLine, m_EditorCurWord;
    QColor m_LinenumbersBackground, m_LinenumbersForeground;
};

StyleItem::StyleItem() :
    d_ptr(new StyleItemPrivate(this))
{

}

const QString &StyleItem::filename() const
{
    Q_D(const StyleItem);
    return d->m_FileName;
}

QTextCharFormat &StyleItem::format(const QString &alias)
{
    Q_D(StyleItem);
    if (!d->m_Formats.contains(alias))
        throw QString("Invalid style format");

    return d->m_Formats[alias];
}

bool StyleItem::hasFormat(const QString &alias) const
{
    Q_D(const StyleItem);
    return d->m_Formats.contains(alias);
}

const QColor &StyleItem::editorBackground() const
{
    Q_D(const StyleItem);
    return d->m_EditorBackground;
}

const QColor &StyleItem::editorCurrentLine() const
{
    Q_D(const StyleItem);
    return d->m_EditorCurLine;
}

const QColor &StyleItem::editorCurrentWord() const
{
    Q_D(const StyleItem);
    return d->m_EditorCurWord;
}

void StyleItem::setEditorBackground(const QColor &color)
{
    Q_D(StyleItem);
    d->m_EditorBackground = color;
}

void StyleItem::setEditorCurrentLine(const QColor &color)
{
    Q_D(StyleItem);
    d->m_EditorCurLine = color;
}

void StyleItem::setEditorCurrentWord(const QColor &color)
{
    Q_D(StyleItem);
    d->m_EditorCurWord = color;
}

const QColor &StyleItem::linenumbersBackground() const
{
    Q_D(const StyleItem);
    return d->m_LinenumbersBackground;
}

const QColor &StyleItem::linenumbersForeground() const
{
    Q_D(const StyleItem);
    return d->m_LinenumbersForeground;
}

void StyleItem::setLinenumbersBackground(const QColor &color)
{
    Q_D(StyleItem);
    d->m_LinenumbersBackground = color;
    qDebug() << color;
}

void StyleItem::setLinenumbersForeground(const QColor &color)
{
    Q_D(StyleItem);
    d->m_LinenumbersForeground = color;
}

#define BOOL_TO_STR(v)(v ? "true" : "false")
void StyleItem::save(const QString &filename)
{
    Q_D(StyleItem);

    if (filename.isEmpty() && d->m_FileName.isEmpty())
        return;

    QJsonObject root;
    QTextCharFormat defaultFormat = format(FormatDefault);

    root.insert("editor.background", d->m_EditorBackground.name());
    root.insert("editor.selection.word", d->m_EditorCurWord.name());
    root.insert("editor.selection.line", d->m_EditorCurLine.name());

    if (d->m_LinenumbersBackground.isValid())
        root.insert("editor.linenumbers.background", d->m_LinenumbersBackground.name());

    if (d->m_LinenumbersForeground.isValid())
        root.insert("editor.linenumbers.foreground", d->m_LinenumbersForeground.name());

    auto WriteFormat = [=](QTextCharFormat Format, const QString &alias) -> QJsonObject
    {
        QJsonObject obj;
        obj.insert("alias", alias);

        if (!Format.fontFamily().isEmpty())
            obj.insert("fontFamily", Format.fontFamily());

        obj.insert("fontFixedPitch", Format.fontFixedPitch());
        obj.insert("fontItalic", BOOL_TO_STR(Format.fontItalic()));
        obj.insert("fontKerning", BOOL_TO_STR(Format.fontKerning()));
        obj.insert("fontLetterSpacing", Format.fontLetterSpacing());
        obj.insert("fontOverline", BOOL_TO_STR(Format.fontOverline()));
        obj.insert("fontPointSize", Format.fontPointSize());
        obj.insert("fontStretch", Format.fontStretch());
        obj.insert("fontStrikeOut", BOOL_TO_STR(Format.fontStrikeOut()));
        obj.insert("fontUnderline", BOOL_TO_STR(Format.fontUnderline()));

        switch(Format.fontWeight())
        {
        case QFont::Thin:
            obj.insert("fontWeight", "Thin");
            break;
        case QFont::ExtraLight:
            obj.insert("fontWeight", "ExtraLight");
            break;
        case QFont::Light:
            obj.insert("fontWeight", "Light");
            break;
        case QFont::Normal:
            obj.insert("fontWeight", "Normal");
            break;
        case QFont::Medium:
            obj.insert("fontWeight", "Medium");
            break;
        case QFont::DemiBold:
            obj.insert("fontWeight", "DemiBold");
            break;
        case QFont::Bold:
            obj.insert("fontWeight", "Bold");
            break;
        case QFont::ExtraBold:
            obj.insert("fontWeight", "ExtraBold");
            break;
        case QFont::Black:
            obj.insert("fontWeight", "Black");
            break;
        default:
            obj.insert("fontWeight", Format.fontWeight());
        }

        obj.insert("fontWordSpacing", Format.fontWordSpacing());

        if (Format.property(QTextFormat::ForegroundBrush).isValid())
            obj.insert("foreground", Format.foreground().color().name());

        if (Format.property(QTextFormat::BackgroundBrush).isValid())
            obj.insert("background", Format.background().color().name());

        if (Format.underlineColor().isValid())
            obj.insert("underlineColor", Format.underlineColor().name());

        if (Format.property(QTextFormat::TextOutline).isValid())
            obj.insert("textOutline", Format.textOutline().color().name());

        switch(Format.underlineStyle())
        {
        case QTextCharFormat::NoUnderline:
            obj.insert("underlineStyle", "NoUnderline");
            break;
        case QTextCharFormat::SingleUnderline:
            obj.insert("underlineStyle", "SingleUnderline");
            break;
        case QTextCharFormat::DashUnderline:
            obj.insert("underlineStyle", "DashUnderline");
            break;
        case QTextCharFormat::DotLine:
            obj.insert("underlineStyle", "DotLine");
            break;
        case QTextCharFormat::DashDotLine:
            obj.insert("underlineStyle", "DashDotLine");
            break;
        case QTextCharFormat::DashDotDotLine:
            obj.insert("underlineStyle", "DashDotDotLine");
            break;
        case QTextCharFormat::WaveUnderline:
            obj.insert("underlineStyle", "WaveUnderline");
            break;
        case QTextCharFormat::SpellCheckUnderline:
            obj.insert("underlineStyle", "SpellCheckUnderline");
            break;
        }

        return obj;
    };

    QJsonArray formats;
    QMapIterator<QString, QTextCharFormat> iter(d->m_Formats);
    while (iter.hasNext())
    {
        iter.next();
        formats.append(WriteFormat(iter.value(), iter.key()));
    }

    root.insert("formats", formats);

    QJsonDocument doc;
    doc.setObject(root);

    QString fname = filename.isEmpty() ? d->m_FileName : filename;

    QFile f(fname);
    if (f.open(QIODevice::WriteOnly))
    {
        f.write(doc.toJson(QJsonDocument::Indented));
        f.close();
    }
}

void StyleItem::load(const QString &filename)
{
    Q_D(StyleItem);
    QFile f(filename);
    f.open(QIODevice::ReadOnly);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &error);

    if (error.error != QJsonParseError::NoError)
        throw error;

    QJsonObject obj = doc.object();

    if (!obj.contains("formats"))
        throw (NO_ELEMENT_TEMPLATE).arg("formats");

    if (obj.contains("editor.background"))
    {
        QString hex = obj["editor.background"].toString();
        CheckColorFormat(hex, "editor.background");
        d->m_EditorBackground = QColor(hex);
    }

    if (obj.contains("editor.selection.line"))
    {
        QString hex = obj["editor.selection.line"].toString();
        CheckColorFormat(hex, "editor.selection.line");
        d->m_EditorCurLine = QColor(hex);
    }

    if (obj.contains("editor.selection.word"))
    {
        QString hex = obj["editor.selection.word"].toString();
        CheckColorFormat(hex, "editor.selection.word");
        d->m_EditorCurWord = QColor(hex);
    }

    if (obj.contains("editor.linenumbers.background"))
    {
        QString hex = obj["editor.linenumbers.background"].toString();
        CheckColorFormat(hex, "editor.linenumbers.background");
        d->m_LinenumbersBackground = QColor(hex);
    }

    if (obj.contains("editor.linenumbers.foreground"))
    {
        QString hex = obj["editor.linenumbers.foreground"].toString();
        CheckColorFormat(hex, "editor.linenumbers.foreground");
        d->m_LinenumbersForeground = QColor(hex);
    }

    auto toBool = [=](const QJsonValue &obj) -> bool
    {
        QString value = obj.toString();

        return value == "true" ? true : false;
    };

    QJsonArray formats = obj["formats"].toArray();

    for (auto format : formats)
    {
        QJsonObject elem = format.toObject();

        if (!elem.contains("alias"))
            throw (NO_ELEMENT_TEMPLATE).arg("alias");

        QString Alias = elem["alias"].toString();

        QTextCharFormat textFormat;
        if (elem.contains("fontFamily"))
            textFormat.setFontFamily(elem["fontFamily"].toString());

        if (elem.contains("fontFixedPitch"))
            textFormat.setFontFixedPitch(toBool(elem["fontFixedPitch"]));

        if (elem.contains("fontItalic"))
            textFormat.setFontItalic(toBool(elem["fontItalic"]));

        if (elem.contains("fontKerning"))
            textFormat.setFontKerning(toBool(elem["fontKerning"]));

        if (elem.contains("fontLetterSpacing"))
            textFormat.setFontLetterSpacing(elem["fontLetterSpacing"].toInt());

        if (elem.contains("fontOverline"))
            textFormat.setFontOverline(toBool(elem["fontOverline"]));

        if (elem.contains("fontPointSize"))
            textFormat.setFontPointSize(elem["fontPointSize"].toInt());

        if (elem.contains("fontStretch"))
            textFormat.setFontStretch(elem["fontStretch"].toInt());

        if (elem.contains("fontStrikeOut"))
            textFormat.setFontStrikeOut(toBool(elem["fontStrikeOut"]));

        if (elem.contains("fontUnderline"))
            textFormat.setFontUnderline(toBool(elem["fontUnderline"]));

        if (elem.contains("fontWeight"))
        {
            int fontWeight = QFont::Normal;
            QString cons = elem["fontWeight"].toString();

            if (cons == "Thin")
                fontWeight = QFont::Thin;
            else if (cons == "ExtraLight")
                fontWeight = QFont::ExtraLight;
            else if (cons == "Light")
                fontWeight = QFont::Light;
            else if (cons == "Normal")
                fontWeight = QFont::Normal;
            else if (cons == "Medium")
                fontWeight = QFont::Medium;
            else if (cons == "DemiBold")
                fontWeight = QFont::DemiBold;
            else if (cons == "Bold")
                fontWeight = QFont::Bold;
            else if (cons == "ExtraBold")
                fontWeight = QFont::ExtraBold;
            else if (cons == "Black")
                fontWeight = QFont::Black;
            else
            {
                if (!elem["fontWeight"].isDouble())
                    throw QString("Invalid \"fontWeight\" value");
                else
                    fontWeight = elem["fontWeight"].toInt();
            }

            textFormat.setFontWeight(fontWeight);
        }

        if (elem.contains("fontWordSpacing"))
            textFormat.setFontWordSpacing(elem["fontWordSpacing"].toInt());

        if (elem.contains("foreground"))
        {
            QString hex = elem["foreground"].toString();
            CheckColorFormat(hex, "foreground");
            textFormat.setForeground(QColor(hex));
        }

        if (elem.contains("background"))
        {
            QString hex = elem["background"].toString();
            CheckColorFormat(hex, "background");
            textFormat.setBackground(QColor(hex));
        }

        if (elem.contains("underlineColor"))
        {
            QString hex = elem["underlineColor"].toString();
            CheckColorFormat(hex, "underlineColor");
            textFormat.setUnderlineColor(QColor(hex));
        }

        if (elem.contains("textOutline"))
        {
            QString hex = elem["textOutline"].toString();
            CheckColorFormat(hex, "textOutline");
            textFormat.setTextOutline(QColor(hex));
        }

        if (elem.contains("underlineStyle"))
        {
            QTextCharFormat::UnderlineStyle underlineStyle = QTextCharFormat::NoUnderline;
            QString cons = elem["underlineStyle"].toString();

            if (cons == "NoUnderline")
                underlineStyle = QTextCharFormat::NoUnderline;
            else if (cons == "SingleUnderline")
                underlineStyle = QTextCharFormat::SingleUnderline;
            else if (cons == "DashUnderline")
                underlineStyle = QTextCharFormat::DashUnderline;
            else if (cons == "DotLine")
                underlineStyle = QTextCharFormat::DotLine;
            else if (cons == "DashDotLine")
                underlineStyle = QTextCharFormat::DashDotLine;
            else if (cons == "DashDotDotLine")
                underlineStyle = QTextCharFormat::DashDotDotLine;
            else if (cons == "WaveUnderline")
                underlineStyle = QTextCharFormat::WaveUnderline;
            else if (cons == "SpellCheckUnderline")
                underlineStyle = QTextCharFormat::SpellCheckUnderline;
            else
                throw QString("Invalid \"underlineStyle\" value");

            textFormat.setUnderlineStyle(underlineStyle);
        }

        textFormat.setFontStyleHint(QFont::Courier);
        (void)d->m_Formats.insert(Alias, textFormat);
    }

    d->m_FileName = filename;
    f.close();
}

// ------------------------------------------------------------

class HighlighterStylePrivate
{
    Q_DECLARE_PUBLIC(HighlighterStyle)
public:
    HighlighterStylePrivate(HighlighterStyle *parent)
    {
        q_ptr = parent;
        m_DefaultTheme = "Default";
    }

    void loadStaticStyle(const QString &source, const QString &alias)
    {
        QSharedPointer<StyleItem> item(new StyleItem());
        item->load(source);
        m_Styles.insert(alias, item);

        m_StaticThemes.append(alias);
        m_Themes.append(alias);
    }

    void loadStyle(const QFileInfo &fi)
    {
        QString name = fi.baseName();

        try
        {
            qInfo(logHighlighter()) << "Try load highlighter style:" << name;
            if (!m_Styles.contains(name))
            {
                QSharedPointer<StyleItem> item(new StyleItem());
                item->load(fi.absoluteFilePath());
                (void)m_Styles.insert(name, item);

                m_Themes.append(name);
            }
            qInfo(logHighlighter()) << "Highlighter style successfully loaded";
        }
        catch (QJsonParseError e)
        {
            qWarning(logHighlighter()) << QString("Can't load highlighter style [%1]: %2")
                                              .arg(name, e.errorString());
        }
        catch (QString e) {
            qWarning(logHighlighter()) << QString("Can't load highlighter style [%1]: %2")
                                              .arg(name, e);
        }
    }

    void loadStyles()
    {
        bool hr = true;
        QDir syntaxhighlighter = QDir::currentPath();

        qInfo(logHighlighter()) << "Try detect syntaxhighlighter folder";
        if (!syntaxhighlighter.cd("syntaxhighlighter"))
        {
            qWarning(logHighlighter()) << QString("Working directory %1: false").arg(syntaxhighlighter.path());
            syntaxhighlighter = QDir(qApp->applicationDirPath());

            if (!syntaxhighlighter.cd("syntaxhighlighter"))
            {
                hr = false;
                qWarning(logHighlighter()) << QString("Application directory %1: false").arg(syntaxhighlighter.path());
            }
            else
                qInfo(logHighlighter()) << QString("Syntaxhighlighter folder: %1").arg(syntaxhighlighter.path());
        }
        else
            qInfo(logHighlighter()) << QString("Syntaxhighlighter folder: %1").arg(syntaxhighlighter.path());

        if (!hr)
        {
            syntaxhighlighter = QDir(qApp->applicationDirPath());
            qInfo(logHighlighter()) << "Creating syntaxhighlighter folder:"
                                    << syntaxhighlighter.mkdir("syntaxhighlighter");
        }

        if (hr)
        {
            QFileInfoList lst = syntaxhighlighter.entryInfoList(QStringList() << "*.json");

            for (const auto &fi : qAsConst(lst))
            {
                loadStyle(fi);
            }
        }
    }

    QMap<QString, QSharedPointer<StyleItem>> m_Styles;
    QStringList m_Themes, m_StaticThemes;
    QString m_DefaultTheme;
    HighlighterStyle *q_ptr;
};

HighlighterStyle::HighlighterStyle() :
    d_ptr(new HighlighterStylePrivate(this))
{
    Q_D(HighlighterStyle);
    d->loadStaticStyle(":/DefaultHighlighterStyle", "Default");
    d->loadStaticStyle(":/VisualStudioLight", "Visual Studio (Light)");
    d->loadStaticStyle(":/VisualStudioDark", "Visual Studio (Dark)");

    d->loadStyles();
}

HighlighterStyle::~HighlighterStyle()
{

}

HighlighterStyle *HighlighterStyle::inst()
{
    if (!m_Inst)
        m_Inst = new HighlighterStyle();

    return m_Inst;
}

void HighlighterStyle::loadStyle(const QString &filename)
{
    Q_D(HighlighterStyle);
    QFileInfo fi(filename);
    d->loadStyle(fi);
}

QSharedPointer<StyleItem> HighlighterStyle::style(const QString &name) const
{
    Q_D(const HighlighterStyle);
    if (name.isEmpty())
        return style(d->m_DefaultTheme); // Default

    if (!d->m_Styles.contains(name))
        return nullptr;

    return d->m_Styles[name];
}

QStringList HighlighterStyle::themes() const
{
    Q_D(const HighlighterStyle);
    return d->m_Themes;
}

void HighlighterStyle::setDefaultTheme(const QString &theme)
{
    Q_D(HighlighterStyle);
    d->m_DefaultTheme = theme;
}

QString HighlighterStyle::defaultTheme() const
{
    Q_D(const HighlighterStyle);
    return d->m_DefaultTheme;
}

bool HighlighterStyle::isStatic(const QString &theme) const
{
    Q_D(const HighlighterStyle);
    return d->m_StaticThemes.contains(theme);
}

void HighlighterStyle::deleteStyle(const QString &theme)
{
    Q_D(HighlighterStyle);

    if (isStatic(theme))
        return;

    QSharedPointer<StyleItem> pstyle = style(theme);

    if (QFile::remove(pstyle->filename()))
    {
        d->m_Themes.removeOne(theme);
        d->m_Styles.remove(theme);
    }
}
