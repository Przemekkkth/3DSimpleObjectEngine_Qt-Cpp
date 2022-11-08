#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec3d.h"
#include <QRgb>

struct Triangle
{
    Vec3d p[3];
    QRgb color;// = qRgb(0,0,0);
};

#endif // TRIANGLE_H
