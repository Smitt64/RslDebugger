QT += core gui widgets testlib

TEMPLATE = app

CONFIG += c++17

DEFINES += TOOLSRUNTIME_STATIC_LINK

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    macrorunner.cpp \
    ../rsldbgclnt/codeeditor/codeeditor.cpp \
    ../rsldbgclnt/codeeditor/codehighlighter.cpp \
    ../rsldbgclnt/codeeditor/cppcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/rslcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/sqlcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/xmlcodehighlighter.cpp \
    ../rsldbgclnt/codeeditor/highlighterstyle.cpp

HEADERS += \
    mainwindow.h \
    macrorunner.h \
    ../rsldbgclnt/codeeditor/codeeditor.h \
    ../rsldbgclnt/codeeditor/codehighlighter.h \
    ../rsldbgclnt/codeeditor/cppcodehighlighter.h \
    ../rsldbgclnt/codeeditor/rslcodehighlighter.h \
    ../rsldbgclnt/codeeditor/sqlcodehighlighter.h \
    ../rsldbgclnt/codeeditor/xmlcodehighlighter.h \
    ../rsldbgclnt/codeeditor/highlighterstyle.h

RESOURCES += \
    ../rsldbgclnt/codeeditor.qrc

INCLUDEPATH += $$PWD/../rsldbgclnt $$PWD/../rslexecutor $$PWD/../rstools/include $$PWD/../rstools/include/panel $$PWD/../rstools/include/bedit

# Ported RslExecutor static library (lib/rslexecutor).
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rslexecutor/release/ -lrslexecutor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rslexecutor/debug/ -lrslexecutor
win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rslexecutor/release/rslexecutor.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rslexecutor/debug/rslexecutor.lib

release: LIBS += -L$$PWD/../rstools/lib/release
debug: LIBS += -L$$PWD/../rstools/lib/debug
# qtmain must come BEFORE rsrtlwm.lib: the CRT looks for WinMain, and
# rsrtlwm.lib also contains a main.obj with WinMain/main entry points.
# If rsrtlwm.lib is searched first, the linker pulls in its main.obj.
win32: LIBS += $$[QT_INSTALL_LIBS]/qtmain.lib
LIBS += -lrsrtlwm -lRSScript -lUser32

# Runtime DLLs must sit next to the executable (installed layout: bin/).
runtime_dlls.files = \
    $$PWD/../rstools/lib/release/rsrtlwm.dll \
    $$PWD/../rstools/lib/release/RSScript.dll \
    $$PWD/../rstools/lib/release/fs32cm.dll \
    $$PWD/../rstools/lib/release/rslocale.dll
runtime_dlls.path = $$PWD/../bin
INSTALLS += runtime_dlls

target.path = $$PWD/../bin
INSTALLS += target
