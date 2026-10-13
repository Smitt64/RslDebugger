QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += RSLDBG_LIBRARY TOOLSRUNTIME_STATIC_LINK

CONFIG += c++11

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

SOURCES += \
    cdebug.cpp \
    cdebugroot.cpp \
    clocals.cpp \
    convert.cpp \
    cqsurvey.cpp \
    csurvey.cpp \
    cwatchv.cpp \
    dbgserver.cpp \
    displayvar.cpp \
    lf/appcfgdata2.cpp \
    lf/dbgexception.cpp \
    models/callstackmodel.cpp \
    rembreakpoints.cpp \
    remexpressions.cpp \
    rsldbg.cpp \
    saver.cpp \
    server/breakpointevent.cpp \
    server/dbgserverbase.cpp \
    server/serveractionevent.cpp \
    server/tools.cpp \
    server/updatedbginfoevent.cpp \
    ui/callstackdockwidget.cpp \
    ui/dbgeditorlinewidgetprovider.cpp \
    ui/events/dbgbreakpointevent.cpp \
    ui/events/dbgtraceevent.cpp \
    ui/mainwindow.cpp \
    ../rsldbgclnt/codeeditor/codeeditor.cpp \
    ../rsldbgclnt/codeeditor/codehighlighter.cpp \
    ../rsldbgclnt/codeeditor/cppcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/rslcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/sqlcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/xmlcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/highlighterstyle.cpp

HEADERS += \
    cdebug.h \
    cdebugroot.h \
    clocals.h \
    convert.h \
    cqsurvey.h \
    csurvey.h \
    cwatchv.h \
    dbg.h \
    dbgserver.h \
    dbgserverproto.h \
    displayvar.h \
    lf/bp_data.h \
    lf/dbgexception.h \
    lf/types.h \
    models/callstackmodel.h \
    packpop.h \
    pshpack1.h \
    rembreakpoints.h \
    remexpressions.h \
    rsldbg_global.h \
    rsldbg.h \
    saver.h \
    server/breakpointevent.h \
    server/dbgserverbase.h \
    server/serveractionevent.h \
    server/updatedbginfoevent.h \
    tracemsg.h \
    ui/callstackdockwidget.h \
    ui/dbgeditorlinewidgetprovider.h \
    ui/events/dbgbreakpointevent.h \
    ui/events/dbgtraceevent.h \
    ui/mainwindow.h \
    ../rsldbgclnt/codeeditor/codeeditor.h \
    ../rsldbgclnt/codeeditor/codehighlighter.h \
    ../rsldbgclnt/codeeditor/cppcodehighlighter.h \
    ../rsldbgclnt/codeeditor/rslcodehighlighter.h \
    ../rsldbgclnt/codeeditor/sqlcodehighlighter.h \
    ../rsldbgclnt/codeeditor/xmlcodehighlighter.h \
    ../rsldbgclnt/codeeditor/highlighterstyle.h

RESOURCES += \
    ../rsldbgclnt/codeeditor.qrc

INCLUDEPATH += $$PWD/../rsldbgclnt

INCLUDEPATH += $$PWD/../rstools/include $$PWD/../rstools/include/panel $$PWD/../rstools/include/bedit

release: LIBS += -L$$PWD/../rstools/lib/release
debug: LIBS += -L$$PWD/../rstools/lib/debug
LIBS += -lRSScript -lrsrtlwm -lrsldlmms -lUser32 -lShell32
LIBS += -lVersion -lGdi32 -lUser32 -lAdvapi32 -lWs2_32

INCLUDEPATH += $$PWD/../loki
DEPENDPATH += $$PWD/../loki

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../loki/release/ -lloki
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../loki/debug/ -lloki
else:unix: LIBS += -L$$OUT_PWD/../loki/ -lloki

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../loki/release/libloki.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../loki/debug/libloki.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../loki/release/loki.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../loki/debug/loki.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../loki/libloki.a

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
target.path = $$PWD/../bin
INSTALLS += target
