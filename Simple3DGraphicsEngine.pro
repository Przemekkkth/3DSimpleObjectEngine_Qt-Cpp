QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/scene.cpp \
    src/view.cpp

HEADERS += \
    src/scene.h \
    src/utils/mat4x4.h \
    src/utils/mesh.h \
    src/utils/triangle.h \
    src/utils/vec3d.h \
    src/utils/vector.h \
    src/view.h

RESOURCES += \
    resource.qrc
