#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QElapsedTimer>

#include "utils/mat4x4.h"
#include "utils/mesh.h"
#include "utils/triangle.h"
#include "utils/vec3d.h"
#include "utils/vector.h"
#include "utils.h"

struct KeyStatus
{
    bool m_pressed = false;
    bool m_held = false;
    bool m_released = false;
};

struct MouseStatus
{
    float m_x = 0.0f;
    float m_y = 0.0f;
    bool m_released = false;
    bool m_pressed = false;
};


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
    QRgb GetColour(float lum);

    void handlePlayerInput();
    void resetStatus();
    KeyStatus* m_keys[256];
    MouseStatus* m_mouse;

    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    float m_deltaTime = 0.0f;
    float m_loopTime  = 0.0f;
    const float m_loopSpeed = int(1000.0f/FPS);

    Mesh meshCube;
    Mat4x4 matProj;
    Vec3d vCamera;

    float fTheta;


    // QGraphicsScene interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // SCENE_H
