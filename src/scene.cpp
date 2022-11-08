#include "scene.h"
#include <QDebug>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRgb>
#include <algorithm>

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0,0, SCREEN_SIZE.width(), SCREEN_SIZE.height());
    OnUserCreated();
    connect(&m_timer, &QTimer::timeout, this, &Scene::loop);
    m_timer.start(int(m_loopSpeed));
    m_elapsedTimer.start();
}


void Scene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    qDebug() << "loopTime" << m_loopTime;
    if( m_loopTime > m_loopSpeed)
    {
        m_loopTime -= m_loopSpeed;
        OnUserUpdated();
    }
}

void Scene::OnUserCreated()
{
    meshCube.tris = {

        // SOUTH
        { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        // EAST
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        // NORTH
        { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        // WEST
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        // TOP
        { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        // BOTTOM
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

    };
    //meshCube.LoadFromObjectFile(":/res/VideoShip.obj");
    // Projection Matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)SCREEN_SIZE.height() / (float)SCREEN_SIZE.width();
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;
}

void Scene::OnUserUpdated()
{
    // Clear Screen
    clear();

    // Set up rotation matrices
    Mat4x4 matRotZ, matRotX;
    fTheta += 0.01f;

    // Rotation Z
    matRotZ.m[0][0] = cosf(fTheta);
    matRotZ.m[0][1] = sinf(fTheta);
    matRotZ.m[1][0] = -sinf(fTheta);
    matRotZ.m[1][1] = cosf(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(fTheta * 0.5f);
    matRotX.m[1][2] = sinf(fTheta * 0.5f);
    matRotX.m[2][1] = -sinf(fTheta * 0.5f);
    matRotX.m[2][2] = cosf(fTheta * 0.5f);
    matRotX.m[3][3] = 1;

    // Store triagles for rastering later
    QVector<Triangle> vecTrianglesToRaster;

    // Draw Triangles
    for (auto tri : meshCube.tris)
    {
        Triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
        MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
        MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
        MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

        // Rotate in X-Axis
        MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
        MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

        // Offset into the screen
        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 2.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 2.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 2.0f;

        // Use Cross-Product to get surface normal
        Vec3d normal, line1, line2;
        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

        normal.x = line1.y * line2.z - line1.z * line2.y;
        normal.y = line1.z * line2.x - line1.x * line2.z;
        normal.z = line1.x * line2.y - line1.y * line2.x;

        // It's normally normal to normalise the normal
        float l = sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        normal.x /= l; normal.y /= l; normal.z /= l;

        if(normal.x * (triTranslated.p[0].x - vCamera.x) +
                normal.y * (triTranslated.p[0].y - vCamera.y) +
                normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f)
        {
            // Illumination
            Vec3d light_direction = { 0.0f, 0.0f, -1.0f };
            float l = sqrtf(light_direction.x*light_direction.x + light_direction.y*light_direction.y + light_direction.z*light_direction.z);
            light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

            // How similar is normal to light direction
            float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
            qDebug() << "dp " << dp;
            // Set color
            triTranslated.color = GetColour(dp);

            // Project triangles from 3D --> 2D
            MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
            MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
            MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
            triProjected.p[0].x *= 0.5f * (float)SCREEN_SIZE.width();
            triProjected.p[0].y *= 0.5f * (float)SCREEN_SIZE.height();
            triProjected.p[1].x *= 0.5f * (float)SCREEN_SIZE.width();
            triProjected.p[1].y *= 0.5f * (float)SCREEN_SIZE.height();
            triProjected.p[2].x *= 0.5f * (float)SCREEN_SIZE.width();
            triProjected.p[2].y *= 0.5f * (float)SCREEN_SIZE.height();

            // Set color
            triProjected.color = GetColour(dp);
            // Store triangle for sorting
            vecTrianglesToRaster.push_back(triProjected);
        }

        // Sort triangles from back to front
        std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2)
        {
            float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
            float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
            return z1 > z2;
        });

        // Rasterize triangle
        QPainterPath path;
        path.moveTo(triProjected.p[0].x, triProjected.p[0].y);

        path.lineTo(triProjected.p[1].x, triProjected.p[1].y);
        path.lineTo(triProjected.p[2].x, triProjected.p[2].y);
        path.lineTo(triProjected.p[0].x, triProjected.p[0].y);
        QGraphicsPathItem* pathItem = new QGraphicsPathItem(path);
        pathItem->setPen(QPen(QBrush(triProjected.color), PIXEL_SIZE.width()));
        pathItem->setBrush(QBrush(triProjected.color));
        addItem(pathItem);


    }
}

void Scene::MultiplyMatrixVector(Vec3d &i, Vec3d &o, Mat4x4 &m)
{
    o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

    if (w != 0.0f)
    {
        o.x /= w; o.y /= w; o.z /= w;
    }
}

QRgb Scene::GetColour(float lum)
{
    int pixel_bw = (int)(4.0f*lum);
    QRgb retVal;
    switch (pixel_bw)
    {
        case 0:
    {
        retVal = qRgb(128, 0, 0);
    }
        break;
    case 1:
    {
        retVal = qRgb(153, 0, 0);
    }
        break;

    case 2:
    {
        retVal = qRgb(185, 0, 0);
    }
        break;
    case 3:
    {
        retVal = qRgb(217, 0, 0);
    }
        break;
    default:
    {
        retVal = qRgb(0, 0, 0);
    }
        break;
    }

    return retVal;
}
