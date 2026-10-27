QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += TOOLSRUNTIME_STATIC_LINK

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

SOURCES += \
    ../3rdparty/SARibbon/SARibbon.cpp \
    callstackdockwidget.cpp \
    dbgclient.cpp \
    dbgeditorlinewidgetprovider.cpp \
    finddialog.cpp \
    logevent.cpp \
    main.cpp \
    mainwindow.cpp \
    windowtitlebar.cpp \
    models/callstackmodel.cpp \
    models/logeventmodel.cpp \
    models/varwatchmodel.cpp \
    stdviewdockwidget.cpp \
    varwatchdlg.cpp \
    varwatchdockwidget.cpp \
    codeeditor/codeeditor.cpp \
    codeeditor/codehighlighter.cpp \
    codeeditor/cppcodehighlighter.cpp \
    codeeditor/rslcodehighlighter.cpp \
    codeeditor/sqlcodehighlighter.cpp \
    codeeditor/xmlcodehighlighter.cpp \
    codeeditor/highlighterstyle.cpp

HEADERS += \
    ../3rdparty/SARibbon/SARibbon.h \
    ../rsldbg/dbgserverproto.h \
    callstackdockwidget.h \
    dbgclient.h \
    dbgeditorlinewidgetprovider.h \
    finddialog.h \
    itemviewiconstyle.h \
    logevent.h \
    mainwindow.h \
    models/callstackmodel.h \
    models/logeventmodel.h \
    models/varwatchmodel.h \
    stdviewdockwidget.h \
    varwatchdlg.h \
    varwatchdockwidget.h \
    windowtitlebar.h \
    codeeditor/codeeditor.h \
    codeeditor/codehighlighter.h \
    codeeditor/cppcodehighlighter.h \
    codeeditor/rslcodehighlighter.h \
    codeeditor/sqlcodehighlighter.h \
    codeeditor/xmlcodehighlighter.h \
    codeeditor/highlighterstyle.h

FORMS += \
    mainwindow.ui \
    varwatchdlg.ui

RESOURCES += \
    icons/icons.qrc \
    res.qrc \
    codeeditor.qrc

win32:RC_FILE += rsldbgclnt.rc

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../rsldbg
INCLUDEPATH += $$PWD/../3rdparty/SARibbon

LIBS += -lUser32
LIBS += -lDwmapi

target.path = $$PWD/../bin
INSTALLS += target
