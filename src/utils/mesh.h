#ifndef MESH_H
#define MESH_H
#include <QVector>
#include "triangle.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVector>

struct Mesh
{
    QVector<Triangle> tris;
    bool LoadFromObjectFile(QString sFilename)
    {
        QFile file(sFilename);
        QTextStream stream(&file);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "Not open " << sFilename;
            return false;
        }
        // Local cache of verts
        tris.clear();
        QVector<Vec3d> verts;
        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            //qDebug() << "line " << line;
            if(line[0] == 'v')
            {
                Vec3d v;
                QStringList data = line.split(" ");
                v.x = data[1].toFloat();
                v.y = data[2].toFloat();
                v.z = data[3].toFloat();
                verts.push_back(v);
            }
            if(line[0] == 'f')
            {
                int f[3];
                QStringList data = line.split(" ");
                f[0] = data[1].toInt();
                f[1] = data[2].toInt();
                f[2] = data[3].toInt();
                tris.push_back({
                                   {verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]}
                               });
            }
        }
        file.close();
        return true;
    }

};

#endif // MESH_H
