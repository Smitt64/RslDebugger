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
    dbgsettings.cpp \
    displayvar.cpp \
    lf/appcfgdata2.cpp \
    lf/dbgexception.cpp \
    rembreakpoints.cpp \
    remexpressions.cpp \
    rsldbg.cpp \
    saver.cpp \
    server/breakpointevent.cpp \
    server/dbgannouncer.cpp \
    server/dbgserverbase.cpp \
    server/serveractionevent.cpp \
    server/tools.cpp \
    server/updatedbginfoevent.cpp

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
    dbgsettings.h \
    displayvar.h \
    lf/bp_data.h \
    lf/dbgexception.h \
    lf/types.h \
    packpop.h \
    pshpack1.h \
    rembreakpoints.h \
    remexpressions.h \
    rsldbg_global.h \
    rsldbg.h \
    saver.h \
    server/breakpointevent.h \
    server/dbgannouncer.h \
    server/dbgserverbase.h \
    server/serveractionevent.h \
    server/updatedbginfoevent.h \
    tracemsg.h

INCLUDEPATH += $$PWD/../rstools/include $$PWD/../rstools/include/panel $$PWD/../rstools/include/bedit

release: LIBS += -L$$PWD/../rstools/lib/release
debug: LIBS += -L$$PWD/../rstools/lib/debug
LIBS += -lRSScript -lrsrtlwm -lrsldlmms -lUser32 -lShell32
LIBS += -lVersion -lGdi32 -lUser32 -lAdvapi32 -lWs2_32 -lIphlpapi

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

# Default rules for deployment.
target.path = $$PWD/../bin
INSTALLS += target
