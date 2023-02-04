#include "scene.h"
#include "utils/matrix.h"
#include <QDebug>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QRgb>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <algorithm>

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(0,0, SCREEN_SIZE.width(), SCREEN_SIZE.height());
    OnUserCreated();
    connect(&m_timer, &QTimer::timeout, this, &Scene::loop);
    m_timer.start(int(m_loopSpeed));
    m_elapsedTimer.start();
    setBackgroundBrush(Qt::black);
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i] = new KeyStatus();
    }
    m_mouse = new MouseStatus();
}

void Scene::preapareScene(QString objFileName)
{
   meshCube.LoadFromObjectFile(objFileName);
   vCamera.x = 0.0f;
   vCamera.y = 0.0f;
   vCamera.z = 0.0f;
}


void Scene::loop()
{
    m_deltaTime = m_elapsedTimer.elapsed();
    m_elapsedTimer.restart();

    m_loopTime += m_deltaTime;
    while( m_loopTime > m_loopSpeed)
    {
        if(meshCube.tris.size())
        {
            handlePlayerInput();
            OnUserUpdated();
        }

        m_loopTime -= m_loopSpeed;
        resetStatus();
    }
}

void Scene::OnUserCreated()
{
    meshCube.LoadFromObjectFile(":/res/videoship.obj");
    matProj = Matrix::MakeProjection(90.0f, (float)SCREEN_SIZE.height() / (float)SCREEN_SIZE.width(), 0.1f, 1000.0f);
}

void Scene::OnUserUpdated()
{
    Mat4x4 matRotZ, matRotX;
    matRotZ = Matrix::MakeRotationZ(fTheta * 0.5f);
    matRotX = Matrix::MakeRotationX(fTheta);

    Mat4x4 matTrans;
    matTrans = Matrix::MakeTranslation(0.0f, 0.0f, 5.0f);

    Mat4x4 matWorld;
    matWorld = Matrix::MakeIdentity();
    matWorld = Matrix::MultiplyMatrix(matRotZ, matRotX);
    matWorld = Matrix::MultiplyMatrix(matWorld, matTrans);

    Vec3d vUp = { 0,1,0 };
    Vec3d vTarget = { 0,0,1 };
    Mat4x4 matCameraRot = Matrix::MakeRotationY(fYaw);
    vLookDir = Matrix::MultiplyVector(matCameraRot, vTarget);
    vTarget = Vector::Add(vCamera, vLookDir);
    Mat4x4 matCamera = Matrix::PointAt(vCamera, vTarget, vUp);

    Mat4x4 matView = Matrix::QuickInverse(matCamera);

    QVector<Triangle> vecTrianglesToRaster;

    clear();
    for (auto tri : meshCube.tris)
    {
        Triangle triProjected, triTransformed, triViewed;
        triTransformed.p[0] = Matrix::MultiplyVector(matWorld, tri.p[0]);
        triTransformed.p[1] = Matrix::MultiplyVector(matWorld, tri.p[1]);
        triTransformed.p[2] = Matrix::MultiplyVector(matWorld, tri.p[2]);

        Vec3d normal, line1, line2;

        line1 = Vector::Sub(triTransformed.p[1], triTransformed.p[0]);
        line2 = Vector::Sub(triTransformed.p[2], triTransformed.p[0]);

        normal = Vector::CrossProduct(line1, line2);

        normal = Vector::Normalise(normal);

        Vec3d vCameraRay = Vector::Sub(triTransformed.p[0], vCamera);

        if (Vector::DotProduct(normal, vCameraRay) < 0.0f)
        {

            Vec3d light_direction = { 0.0f, 1.0f, -1.0f };
            light_direction = Vector::Normalise(light_direction);

            float dp = std::max(0.1f, Vector::DotProduct(light_direction, normal));
            triTransformed.color = GetColour(dp);

            triViewed.p[0] = Matrix::MultiplyVector(matView, triTransformed.p[0]);
            triViewed.p[1] = Matrix::MultiplyVector(matView, triTransformed.p[1]);
            triViewed.p[2] = Matrix::MultiplyVector(matView, triTransformed.p[2]);
            triViewed.color = GetColour(dp);

            int nClippedTriangles = 0;
            Triangle clipped[2];
            nClippedTriangles = Triangle::ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

            for (int n = 0; n < nClippedTriangles; n++)
            {
                triProjected.p[0] = Matrix::MultiplyVector(matProj, clipped[n].p[0]);
                triProjected.p[1] = Matrix::MultiplyVector(matProj, clipped[n].p[1]);
                triProjected.p[2] = Matrix::MultiplyVector(matProj, clipped[n].p[2]);
                triProjected.color = clipped[n].color;

                triProjected.p[0] = Vector::Div(triProjected.p[0], triProjected.p[0].w);
                triProjected.p[1] = Vector::Div(triProjected.p[1], triProjected.p[1].w);
                triProjected.p[2] = Vector::Div(triProjected.p[2], triProjected.p[2].w);

                triProjected.p[0].x *= -1.0f;
                triProjected.p[1].x *= -1.0f;
                triProjected.p[2].x *= -1.0f;
                triProjected.p[0].y *= -1.0f;
                triProjected.p[1].y *= -1.0f;
                triProjected.p[2].y *= -1.0f;

                Vec3d vOffsetView = { 1,1,0 };
                triProjected.p[0] = Vector::Add(triProjected.p[0], vOffsetView);
                triProjected.p[1] = Vector::Add(triProjected.p[1], vOffsetView);
                triProjected.p[2] = Vector::Add(triProjected.p[2], vOffsetView);
                triProjected.p[0].x *= 0.5f * (float)SCREEN_SIZE.width();
                triProjected.p[0].y *= 0.5f * (float)SCREEN_SIZE.height();
                triProjected.p[1].x *= 0.5f * (float)SCREEN_SIZE.width();
                triProjected.p[1].y *= 0.5f * (float)SCREEN_SIZE.height();
                triProjected.p[2].x *= 0.5f * (float)SCREEN_SIZE.width();
                triProjected.p[2].y *= 0.5f * (float)SCREEN_SIZE.height();

                vecTrianglesToRaster.push_back(triProjected);
            }

        }


    }

    std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](Triangle &t1, Triangle &t2)
    {
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
    });

    for (auto &triToRaster : vecTrianglesToRaster)
    {
        Triangle clipped[2];
        std::list<Triangle> listTriangles;

        listTriangles.push_back(triToRaster);
        int nNewTriangles = 1;

        for (int p = 0; p < 4; p++)
        {
            int nTrisToAdd = 0;
            while (nNewTriangles > 0)
            {
                Triangle test = listTriangles.front();
                listTriangles.pop_front();
                nNewTriangles--;

                switch (p)
                {
                case 0:	nTrisToAdd = Triangle::ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 1:	nTrisToAdd = Triangle::ClipAgainstPlane({ 0.0f, (float)SCREEN_SIZE.height() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 2:	nTrisToAdd = Triangle::ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                case 3:	nTrisToAdd = Triangle::ClipAgainstPlane({ (float)SCREEN_SIZE.width() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                }

                for (int w = 0; w < nTrisToAdd; w++)
                    listTriangles.push_back(clipped[w]);
            }
            nNewTriangles = listTriangles.size();
        }


        for (auto &triProjected : listTriangles)
        {
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

    for (auto &triProjected : vecTrianglesToRaster)
    {
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
    if(pixel_bw < 0.0f)
    {
        pixel_bw *= -1;
    }

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
    case 4:
    {
        retVal = qRgb(230, 0, 0);
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

void Scene::handlePlayerInput()
{
    float fElapsedTime = 1.0/m_loopTime;

    if(m_keys[KEYBOARD::KEY_I]->m_held)
    {
        vCamera.y += 8.0f * fElapsedTime;
    }
    if(m_keys[KEYBOARD::KEY_K]->m_held)
    {
        vCamera.y -= 8.0f * fElapsedTime;
    }
    if(m_keys[KEYBOARD::KEY_J]->m_held)
    {
        vCamera.x -= 8.0f * fElapsedTime;
    }
    if(m_keys[KEYBOARD::KEY_L]->m_held)
    {
        vCamera.x += 8.0f * fElapsedTime;
    }

    Vec3d vForward = Vector::Mul(vLookDir, 8.0f * fElapsedTime);

    if(m_keys[KEYBOARD::KEY_W]->m_held)
    {
        vCamera = Vector::Add(vCamera, vForward);
    }
    if(m_keys[KEYBOARD::KEY_S]->m_held)
    {
        vCamera = Vector::Sub(vCamera, vForward);
    }
    if(m_keys[KEYBOARD::KEY_A]->m_held)
    {
        fYaw -= 2.0f * fElapsedTime;
    }
    if(m_keys[KEYBOARD::KEY_D]->m_held)
    {
        fYaw += 2.0f * fElapsedTime;
    }

}

void Scene::resetStatus()
{
    for(int i = 0; i < 256; ++i)
    {
        m_keys[i]->m_released = false;
    }
    m_mouse->m_released = false;
}

void Scene::keyPressEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
        }
        else
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = true;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held    = false;
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

void Scene::keyReleaseEvent(QKeyEvent *event)
{
    if(KEYBOARD::KeysMapper.contains(event->key()))
    {
        if(!event->isAutoRepeat())
        {
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_held = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_pressed = false;
            m_keys[KEYBOARD::KeysMapper[event->key()]]->m_released = true;
        }

    }
    QGraphicsScene::keyReleaseEvent(event);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = true;
    QGraphicsScene::mousePressEvent(event);
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouse->m_x = event->scenePos().x();
    m_mouse->m_y = event->scenePos().y();
    m_mouse->m_pressed = false;
    m_mouse->m_released = true;
    QGraphicsScene::mouseReleaseEvent(event);
}
