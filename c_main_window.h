#ifndef C_MAIN_WINDOW_H
#define C_MAIN_WINDOW_H

#include <QWindow>
#include <QTime>

#include "c_voxel_scene.h"

class QOpenGLContext;

class CMainWindow : public QWindow
{
    Q_OBJECT

public:
    CMainWindow( QScreen* screen = 0 );

private:
    void initializeGL();

protected slots:
    void resizeGL();
    void paintGL();
    void updateScene();

protected:
    void keyPressEvent( QKeyEvent* e );
    void keyReleaseEvent( QKeyEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );

private:
    QOpenGLContext* m_context;
    CVoxelScene* m_scene;
    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
    QTime m_time;
};

#endif // C_MAIN_WINDOW_H
