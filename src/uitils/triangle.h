#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vec3d.h"
#include <QRgb>
struct Triangle
{
    Vec3d p[3];
    QRgb color;
};

#endif // TRIANGLE_H
