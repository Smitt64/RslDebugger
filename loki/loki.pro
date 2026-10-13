CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

UI_DIR = build
MOC_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    readme.txt

INCLUDEPATH += $$PWD/loki

SOURCES += \
    LevelMutex.cpp \
    OrderedStatic.cpp \
    SafeFormat.cpp \
    Singleton.cpp \
    SmallObj.cpp \
    SmartPtr.cpp \
    StrongPtr.cpp
