# Статическая библиотека клиента отладочного протокола rsldbg (протокол v3:
# DBGHEADER + JSON по TCP, авторизация токеном после handshake).
# UI-free: QObject + QTcpSocket + виртуальные обработчики.
# Потребители: GUI-клиент (rsldbgclnt) и MCP-сервер (rsldbgmcp).
QT += core network
QT -= gui

TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11


UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/../rsldbg

SOURCES += \
    dbgclient.cpp

HEADERS += \
    ../rsldbg/dbgserverproto.h \
    dbgclient.h
