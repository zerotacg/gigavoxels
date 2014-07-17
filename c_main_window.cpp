#include "c_main_window.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QTimer>

//------------------------------------------------------------------------------
CMainWindow::CMainWindow( QScreen* screen )
    : QWindow( screen ),
      m_scene( new CVoxelScene( this ) ),
      m_leftButtonPressed( false )
{
    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );

    // Specify the format we wish to use
    QSurfaceFormat format;
    format.setDepthBufferSize( 24 );
    format.setMajorVersion( 4 );
    format.setMinorVersion( 3 );
    format.setSamples( 4 );
    format.setProfile( QSurfaceFormat::CoreProfile );
    //format.setOption( QSurfaceFormat::DebugContext );

    resize( 1366, 768 );
    setFormat( format );
    create();

    // Create an OpenGL context
    m_context = new QOpenGLContext;
    m_context->setFormat( format );
    m_context->create();

    // Setup our scene
    m_context->makeCurrent( this );
    m_scene->setContext( m_context );
    initializeGL();

    // Make sure we tell OpenGL about new window sizes
    connect( this, SIGNAL( widthChanged( int ) ), this, SLOT( resizeGL() ) );
    connect( this, SIGNAL( heightChanged( int ) ), this, SLOT( resizeGL() ) );
    resizeGL();

    // This timer drives the scene updates
    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), this, SLOT( updateScene() ) );
    timer->start( 16 );
}

//------------------------------------------------------------------------------
void
CMainWindow::initializeGL()
{
    m_context->makeCurrent( this );
    m_scene->initialise();
    m_time.start();
}

//------------------------------------------------------------------------------
void
CMainWindow::paintGL()
{
    // Make the context current
    m_context->makeCurrent( this );

    // Do the rendering (to the back buffer)
    m_scene->render();

    // Swap front/back buffers
    m_context->swapBuffers( this );
}

//------------------------------------------------------------------------------
void
CMainWindow::resizeGL()
{
    m_context->makeCurrent( this );
    m_scene->resize( width(), height() );
}

//------------------------------------------------------------------------------
void
CMainWindow::updateScene()
{
    float time = m_time.elapsed() / 1000.0f;
    m_scene->update( time );
    paintGL();
}

//------------------------------------------------------------------------------
void
CMainWindow::keyPressEvent( QKeyEvent* e )
{
    const float speed = 44.7f; // in m/s. Equivalent to 100 miles/hour
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QCoreApplication::instance()->quit();
            break;

        case Qt::Key_D:
            m_scene->setSideSpeed( speed );
            break;

        case Qt::Key_A:
            m_scene->setSideSpeed( -speed );
            break;

        case Qt::Key_W:
            m_scene->setForwardSpeed( speed );
            break;

        case Qt::Key_S:
            m_scene->setForwardSpeed( -speed );
            break;

        case Qt::Key_PageUp:
            m_scene->setVerticalSpeed( speed );
            break;

        case Qt::Key_PageDown:
            m_scene->setVerticalSpeed( -speed );
            break;

        case Qt::Key_Shift:
            m_scene->setViewCenterFixed( true );
            break;

        case Qt::Key_Plus:
            break;

        case Qt::Key_Minus:
            break;

        case Qt::Key_Home:
            break;

        case Qt::Key_End:
            break;

        case Qt::Key_BracketLeft:
            break;

        case Qt::Key_BracketRight:
            break;

        case Qt::Key_Comma:
            break;

        case Qt::Key_Period:
            break;

        case Qt::Key_F1:
            break;

        case Qt::Key_F2:
            break;

        case Qt::Key_F3:
            break;

        case Qt::Key_F4:
            break;

        case Qt::Key_F5:
            break;

        case Qt::Key_F6:
            break;

        case Qt::Key_F7:
            break;

        case Qt::Key_F8:
            break;

        default:
            QWindow::keyPressEvent( e );
    }
}

//------------------------------------------------------------------------------
void
CMainWindow::keyReleaseEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_D:
        case Qt::Key_A:
            m_scene->setSideSpeed( 0.0f );
            break;

        case Qt::Key_W:
        case Qt::Key_S:
            m_scene->setForwardSpeed( 0.0f );
            break;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            m_scene->setVerticalSpeed( 0.0f );
            break;

        case Qt::Key_Shift:
            m_scene->setViewCenterFixed( false );
            break;

        default:
            QWindow::keyReleaseEvent( e );
    }
}

//------------------------------------------------------------------------------
void
CMainWindow::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = true;
        m_pos = m_prevPos = e->pos();
    }
    QWindow::mousePressEvent( e );
}

//------------------------------------------------------------------------------
void
CMainWindow::mouseReleaseEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
        m_leftButtonPressed = false;
    QWindow::mouseReleaseEvent( e );
}

//------------------------------------------------------------------------------
void
CMainWindow::mouseMoveEvent( QMouseEvent* e )
{
    if ( m_leftButtonPressed )
    {
        m_pos = e->pos();
        float dx = 0.2f * ( m_pos.x() - m_prevPos.x() );
        float dy = -0.2f * ( m_pos.y() - m_prevPos.y() );
        m_prevPos = m_pos;

        m_scene->pan( dx );
        m_scene->tilt( dy );
    }

    QWindow::mouseMoveEvent( e );

}

//------------------------------------------------------------------------------
