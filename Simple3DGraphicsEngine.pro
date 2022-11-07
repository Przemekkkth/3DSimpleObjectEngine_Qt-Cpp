QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/scene.cpp \
    src/view.cpp

HEADERS += \
    src/scene.h \
    src/uitils/mat4x4.h \
    src/uitils/mesh.h \
    src/uitils/triangle.h \
    src/uitils/vec3d.h \
    src/view.h
