TEMPLATE = app

CONFIG += console

QT += core gui widgets

CONFIG += c++11

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

INCLUDEPATH += $$PWD/../rstools/include $$PWD/../rstools/include/panel $$PWD/../rstools/include/bedit

release: LIBS += -L$$PWD/../rstools/lib/release
debug: LIBS += -L$$PWD/../rstools/lib/debug
LIBS += -lrsrtlwm -lRSScript -lUser32

SOURCES += \
    main.cpp

# Runtime DLLs (rsrtlwm, RSScript, fs32cm, rslocale) must sit next to the
# executable together with rsldbg.dll / rsldbgclnt.exe (installed by rsldbg).
runtime_dlls.files = \
    $$PWD/../rstools/lib/release/rsrtlwm.dll \
    $$PWD/../rstools/lib/release/RSScript.dll \
    $$PWD/../rstools/lib/release/fs32cm.dll \
    $$PWD/../rstools/lib/release/rslocale.dll
runtime_dlls.path = $$PWD/../bin
INSTALLS += runtime_dlls

target.path = $$PWD/../bin
INSTALLS += target
