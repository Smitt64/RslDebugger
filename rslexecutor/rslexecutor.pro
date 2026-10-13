# Статическая библиотека: перенос RslExecutor (+ каталог rsscript) из
# RsWorkTools/ToolsRuntime. toolsruntime.h/.cpp здесь - минимальный аналог
# оригинального (только logRsl и toolFullFileNameFromDir).
QT += core gui widgets testlib

TEMPLATE = lib

CONFIG += staticlib c++17

# Статическая линковка: отключает Q_DECL_EXPORT/IMPORT.
DEFINES += TOOLSRUNTIME_STATIC_LINK Q_WS_WIN

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

SOURCES += \
    rslexecutor.cpp \
    toolsruntime.cpp \
    rsscript/Methods.cpp \
    rsscript/Properties.cpp \
    rsscript/Signals.cpp \
    rsscript/registerinfobase.cpp \
    rsscript/registerobjlist.cpp \
    rsscript/rsllistitemcontainer.cpp \
    rsscript/rsllistwrapper.cpp \
    rsscript/rslobjconnections.cpp \
    rsscript/rslstaticmodule.cpp \
    rsscript/typeinfofunc.cpp \
    rslmodule/types/QByteArrayRsl.cpp \
    rslmodule/types/point.cpp \
    rslmodule/types/rect.cpp \
    rslmodule/types/size.cpp

HEADERS += \
    rslexecutor.h \
    toolsruntime.h \
    ToolsRuntime_global.h \
    rsscript/RslModulePluginInterface.h \
    rsscript/TRsbRSLInstTmpl.hpp \
    rsscript/registerinfobase.h \
    rsscript/registerobjlist.hpp \
    rsscript/rslibdynamicfuncs.h \
    rsscript/rsllistwrapper.h \
    rsscript/rslobjconnections.h \
    rsscript/rslstaticmodule.h \
    rsscript/typeinfo_p.h

INCLUDEPATH += $$PWD $$PWD/rsscript
INCLUDEPATH += $$PWD/../rstools/include $$PWD/../rstools/include/panel $$PWD/../rstools/include/bedit

release: LIBS += -L$$PWD/../rstools/lib/release
debug: LIBS += -L$$PWD/../rstools/lib/debug
LIBS += -lrsrtlwm -lRSScript -lUser32
