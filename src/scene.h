#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QElapsedTimer>

#include "uitils/mat4x4.h"
#include "uitils/mesh.h"
#include "uitils/triangle.h"
#include "uitils/vec3d.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit Scene(QObject *parent = nullptr);

    QSize PIXEL_SIZE  = QSize(4,4);
    QSize WINDOW_SIZE = QSize(256, 240);
    QSize SCREEN_SIZE = QSize(PIXEL_SIZE.width()*WINDOW_SIZE.width(),
                              PIXEL_SIZE.height()*WINDOW_SIZE.height());
    const int FPS = 60;
signals:


private slots:
    void loop();
private:
    void OnUserCreated();
    void OnUserUpdated();
    void MultiplyMatrixVector(Vec3d &i, Vec3d &o, Mat4x4 &m);

    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    float m_deltaTime = 0.0f;
    float m_loopTime  = 0.0f;
    const float m_loopSpeed = int(1000.0f/FPS);

    Mesh meshCube;
    Mat4x4 matProj;

    float fTheta;

};

#endif // SCENE_H
