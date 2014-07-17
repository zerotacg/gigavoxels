#include "c_voxel_scene.h"
#include "camera.h"

#include <string.h>

#include <QImage>
#include <QGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions_4_0_Core>

#define _USE_MATH_DEFINES
#include <math.h>

//------------------------------------------------------------------------------
const float degToRad = float( M_PI / 180.0 );

//------------------------------------------------------------------------------
CVoxelScene::CVoxelScene( QObject* parent )
    : AbstractScene( parent ),
      m_camera( new Camera( this ) ),
      m_v(),
      m_viewCenterFixed( false ),
      m_panAngle( 0.0f ),
      m_tiltAngle( 0.0f ),
      m_modelMatrix(),
      m_time( 0.0f ),
      m_metersToUnits( 0.5f ), // 500 units == 10 km => 0.05 units/m
      m_funcs( NULL )
{
    m_modelMatrix.setToIdentity();

    // Initialize the camera position and orientation
    const float height( 10.0 );
    m_camera->setPosition( QVector3D( 0.0f, height, 0.0f ) );
    m_camera->setViewCenter( QVector3D( 1.0f, height, 1.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

//------------------------------------------------------------------------------
void
CVoxelScene::initialise()
{
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_4_0_Core>();
    if ( !m_funcs )
    {
        qFatal("Requires OpenGL >= 4.0");
        exit( 1 );
    }
    m_funcs->initializeOpenGLFunctions();

    // Initialize resources
    prepareShaders();

    // Enable depth testing
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );

    glClearColor( 0.65f, 0.77f, 1.0f, 1.0f );

    // Set the wireframe line properties
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();
}

//------------------------------------------------------------------------------
void
CVoxelScene::update( float t )
{
    m_modelMatrix.setToIdentity();

    // Store the time
    const float dt = t - m_time;
    m_time = t;

    // Update the camera position and orientation
    Camera::CameraTranslationOption option = m_viewCenterFixed
                                           ? Camera::DontTranslateViewCenter
                                           : Camera::TranslateViewCenter;
    m_camera->translate( m_v * dt * m_metersToUnits, option );

    if ( !qFuzzyIsNull( m_panAngle ) )
    {
        m_camera->pan( m_panAngle, QVector3D( 0.0f, 1.0f, 0.0f ) );
        m_panAngle = 0.0f;
    }

    if ( !qFuzzyIsNull( m_tiltAngle ) )
    {
        m_camera->tilt( m_tiltAngle );
        m_tiltAngle = 0.0f;
    }
}

//------------------------------------------------------------------------------
void
CVoxelScene::render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_material->bind();
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    // Pass in the usual transformation matrices
    QMatrix4x4 viewMatrix = m_camera->viewMatrix();
    QMatrix4x4 modelViewMatrix = viewMatrix * m_modelMatrix;
    QMatrix3x3 worldNormalMatrix = m_modelMatrix.normalMatrix();
    QMatrix3x3 normalMatrix = modelViewMatrix.normalMatrix();
    QMatrix4x4 mvp = m_camera->projectionMatrix() * modelViewMatrix;
    shader->setUniformValue( "modelMatrix", m_modelMatrix );
    shader->setUniformValue( "modelViewMatrix", modelViewMatrix );
    shader->setUniformValue( "worldNormalMatrix", worldNormalMatrix );
    shader->setUniformValue( "normalMatrix", normalMatrix );
    shader->setUniformValue( "mvp", mvp );

    // Render the quad as a patch
    {
        //QOpenGLVertexArrayObject::Binder binder( &m_vao );
        //shader->setPatchVertexCount( 1 );
        //glDrawArrays( GL_PATCHES, 0, m_patchCount );
    }
}

//------------------------------------------------------------------------------
void
CVoxelScene::resize( int w, int h )
{
    // Make sure the viewport covers the entire window
    glViewport( 0, 0, w, h );

    m_viewportSize = QVector2D( float( w ), float( h ) );

    // Update the projection matrix
    float aspect = static_cast<float>( w ) / static_cast<float>( h );
    m_camera->setPerspectiveProjection( 25.0f, aspect, 0.1f, 10240.0f );

    // Update the viewport matrix
    float w2 = w / 2.0f;
    float h2 = h / 2.0f;
    m_viewportMatrix.setToIdentity();
    m_viewportMatrix.setColumn( 0, QVector4D( w2, 0.0f, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 1, QVector4D( 0.0f, h2, 0.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 2, QVector4D( 0.0f, 0.0f, 1.0f, 0.0f ) );
    m_viewportMatrix.setColumn( 3, QVector4D( w2, h2, 0.0f, 1.0f ) );

    // We need the viewport size to calculate tessellation levels
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->setUniformValue( "viewportSize", m_viewportSize );

    // The geometry shader also needs the viewport matrix
    shader->setUniformValue( "viewportMatrix", m_viewportMatrix );
}

//------------------------------------------------------------------------------
void
CVoxelScene::prepareShaders()
{
    m_material = MaterialPtr( new Material );
    m_material->setShaders( "shaders/gigavoxels.vert",
                            "shaders/gigavoxels.frag" );
}

//------------------------------------------------------------------------------
void
CVoxelScene::prepareTextures()
{
    SamplerPtr sampler( new Sampler );
    sampler->create();
    sampler->setMinificationFilter( GL_LINEAR );
    sampler->setMagnificationFilter( GL_LINEAR );
    sampler->setWrapMode( Sampler::DirectionS, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionT, GL_CLAMP_TO_EDGE );
    sampler->setWrapMode( Sampler::DirectionR, GL_CLAMP_TO_EDGE );

    m_funcs->glActiveTexture( GL_TEXTURE0 );
    TexturePtr brick( new QOpenGLTexture( QOpenGLTexture::Target3D ) );
    brick->setAutoMipMapGenerationEnabled( false );
    int width( 16 ), height( width ), depth( width );
    brick->setSize( width, height, depth );
    brick->allocateStorage();
    m_material->setTextureUnitConfiguration( 0, brick, sampler, QByteArrayLiteral( "brick" ) );

    m_funcs->glActiveTexture( GL_TEXTURE0 );
}

//------------------------------------------------------------------------------
