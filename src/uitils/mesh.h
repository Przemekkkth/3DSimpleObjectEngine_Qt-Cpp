#ifndef MESH_H
#define MESH_H
#include <QVector>
#include "triangle.h"

struct Mesh
{
    QVector<Triangle> tris;
};

#endif // MESH_H
