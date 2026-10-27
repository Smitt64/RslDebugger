QT += core network
QT -= gui

# winsock для discovery (UDP-пробы анонсера)
win32: LIBS += -lws2_32

CONFIG += c++11 console
CONFIG -= app_bundle


TARGET = rsldbgmcp

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../rsldbg
INCLUDEPATH += $$PWD/../dbgclient

SOURCES += \
    discovery.cpp \
    main.cpp \
    mcpdbgclient.cpp \
    mcpserver.cpp

HEADERS += \
    ../rsldbg/dbgserverproto.h \
    ../dbgclient/dbgclient.h \
    discovery.h \
    mcpdbgclient.h \
    mcpserver.h

# Клиент отладочного протокола (статическая библиотека).
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../dbgclient/release/ -ldbgclient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../dbgclient/debug/ -ldbgclient
win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbgclient/release/dbgclient.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../dbgclient/debug/dbgclient.lib

target.path = $$PWD/../bin
INSTALLS += target
