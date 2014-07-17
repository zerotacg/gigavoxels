#ifndef C_AREA_SCENE_H
#define C_AREA_SCENE_H

#include "abstractscene.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

class Camera;

class QOpenGLFunctions_4_0_Core;

class CVoxelScene : public AbstractScene
{
    Q_OBJECT

public:
    CVoxelScene( QObject* parent = 0 );

    virtual void initialise();
    virtual void update( float t );
    virtual void render();
    virtual void resize( int w, int h );

    // Camera motion control
    void setSideSpeed( float vx ) { m_v.setX( vx ); }
    void setVerticalSpeed( float vy ) { m_v.setY( vy ); }
    void setForwardSpeed( float vz ) { m_v.setZ( vz ); }
    void setViewCenterFixed( bool b ) { m_viewCenterFixed = b; }

    // Camera orientation control
    void pan( float angle ) { m_panAngle = angle; }
    void tilt( float angle ) { m_tiltAngle = angle; }

private:
    void prepareShaders();
    void prepareTextures();

    Camera* m_camera;
    QVector3D m_v;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    QMatrix4x4 m_viewportMatrix;
    QMatrix4x4 m_modelMatrix;
    QVector2D m_viewportSize;

    MaterialPtr m_material;

    float m_time;
    const float m_metersToUnits;

    QOpenGLFunctions_4_0_Core* m_funcs;
};

#endif // C_AREA_SCENE_H
