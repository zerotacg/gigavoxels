#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T19:45:22
#
#-------------------------------------------------

include( common/common.pri )

TARGET = gigavoxels
TEMPLATE = app
#CONFIG += c++11

INCLUDEPATH += common

SOURCES += main.cpp \
    c_main_window.cpp \
    c_voxel_scene.cpp

HEADERS  += \
    c_main_window.h \
    c_voxel_scene.h

OTHER_FILES += \
    common/common.pri \
    CREDITS.md \
    README.md \
    shaders/gigavoxels.frag \
    shaders/gigavoxels.vert
