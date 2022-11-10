#ifndef VECTOR_H
#define VECTOR_H
#include <cmath>
#include "vec3d.h"

struct Vector
{
    static inline Vec3d Add(Vec3d &v1, Vec3d &v2)
    {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    }

    static inline Vec3d Sub(Vec3d &v1, Vec3d &v2)
    {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    }

    static inline Vec3d Mul(Vec3d &v1, float k)
    {
        return { v1.x * k, v1.y * k, v1.z * k };
    }

    static inline Vec3d Div(Vec3d &v1, float k)
    {
        return { v1.x / k, v1.y / k, v1.z / k };
    }

    static inline float DotProduct(Vec3d &v1, Vec3d &v2)
    {
        return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
    }

    static inline float Length(Vec3d &v)
    {
        return sqrtf( Vector::DotProduct(v,v) );
    }

    static inline Vec3d Normalise(Vec3d &v)
    {
        float l = Vector::Length(v);
        return { v.x / l, v.y / l, v.z / l };
    }

    static inline Vec3d CrossProduct(Vec3d &v1, Vec3d &v2)
    {
        Vec3d v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
    }

    static inline Vec3d IntersectPlane(Vec3d &plane_p, Vec3d &plane_n, Vec3d &lineStart, Vec3d &lineEnd)
    {
        plane_n = Vector::Normalise(plane_n);
        float plane_d = -Vector::DotProduct(plane_n, plane_p);
        float ad = Vector::DotProduct(lineStart, plane_n);
        float bd = Vector::DotProduct(lineEnd, plane_n);
        float t = (-plane_d - ad) / (bd - ad);
        Vec3d lineStartToEnd = Vector::Sub(lineEnd, lineStart);
        Vec3d lineToIntersect = Vector::Mul(lineStartToEnd, t);
        return Vector::Add(lineStart, lineToIntersect);
    }
};

#endif // VECTOR_H
