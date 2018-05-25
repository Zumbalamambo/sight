/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/FrameBufferItem.hpp"

#include "fwRenderVTK/vtkInternalOpenGLRenderWindow.hpp"

#include <fwGuiQt/QtQmlHelper.hpp>

#include <QMimeData>
#include <QOpenGLFramebufferObject>
#include <QQuickFramebufferObject>
#include <QQuickWindow>
#include <QVTKInteractor.h>
#include <QVTKInteractorAdapter.h>
#include <vtkCamera.h>
#include <vtkConeSource.h>
#include <vtkImageActor.h>
#include <vtkImageMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

namespace fwRenderVTK
{

static ::fwServices::QtQmlType<FrameBufferItem>    registar("com.fw4spl.vtk", 1, 0, "FrameBuffer");

FrameBufferRenderer::FrameBufferRenderer(vtkInternalOpenGLRenderWindow* rw, FrameBufferItem* item) :
    m_vtkRenderWindow(rw),
    m_framebufferObject(0),
    m_item(item),
    m_readyToRender(false)
{
    m_vtkRenderWindow->Register(NULL);
    m_vtkRenderWindow->setRenderer(this);
}

//-----------------------------------------------------------------------------

FrameBufferRenderer::~FrameBufferRenderer()
{
    m_vtkRenderWindow->setRenderer(0);
    m_vtkRenderWindow->Delete();
}

//-----------------------------------------------------------------------------

QOpenGLFramebufferObject* FrameBufferRenderer::createFramebufferObject(const QSize& size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setInternalTextureFormat(GL_RGBA);
    format.setSamples(0);
    m_framebufferObject = new QOpenGLFramebufferObject(size, format);
    m_vtkRenderWindow->setFrameBufferObject(m_framebufferObject);
    m_item->ready();
    return m_framebufferObject;
}

//------------------------------------------------------------------------------------------

void FrameBufferRenderer::render()
{
    if (!m_readyToRender)
    {
        return;
    }

    m_item->lockRenderer();
    m_vtkRenderWindow->Start();
    m_vtkRenderWindow->internalRender(); // vtkXOpenGLRenderWindow renders the scene to the FBO

    m_item->unlockRenderer();
}

//------------------------------------------------------------------------------

FrameBufferItem const* FrameBufferRenderer::getItem() const
{
    return m_item;
}

//-----------------------------------------------------------------------------

void FrameBufferRenderer::synchronize(QQuickFramebufferObject* item)
{
    if (!m_framebufferObject)
    {
        FrameBufferItem* vtkItem = static_cast<FrameBufferItem*>(item);
        vtkItem->initialize();
        m_readyToRender = true;

    }
}

//------------------------------------------------------------------------------------------

FrameBufferItem::FrameBufferItem(QQuickItem* parent) :
    QQuickFramebufferObject(parent),
    m_win(nullptr),
    m_interactorAdapter(nullptr)
{
    setMirrorVertically(true);
    setAcceptTouchEvents(true);
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    this->setAcceptedMouseButtons(Qt::AllButtons);
    m_win = vtkInternalOpenGLRenderWindow::New();
    m_win->SetSize(width(), height());
    m_win->AddRenderer(m_renderer);
    m_interactor = QVTKInteractor::New();
    m_win->SetInteractor(m_interactor);
    m_interactorAdapter = new QVTKInteractorAdapter(this);
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
        vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_interactor->SetInteractorStyle(style);
}

//-----------------------------------------------------------------------------

FrameBufferItem::~FrameBufferItem()
{
    if (m_win)
    {
        m_win->Delete();
    }
}

//-----------------------------------------------------------------------------

vtkInternalOpenGLRenderWindow* FrameBufferItem::getRenderWindow() const
{
    return m_win;
}

//-----------------------------------------------------------------------------

QQuickFramebufferObject::Renderer* FrameBufferItem::createRenderer() const
{
    std::cout << "CREATE AND CONNECT RENDERER" << std::endl;
    auto renderer =
        new FrameBufferRenderer(static_cast<vtkInternalOpenGLRenderWindow*>(m_win), const_cast<FrameBufferItem*>(this));
    connect(renderer, SIGNAL(ready()), this, SIGNAL(ready()));
    m_interactor->Initialize();
    return renderer;
}

//-----------------------------------------------------------------------------

vtkSmartPointer<vtkRenderer>    FrameBufferItem::getRenderer() const
{
    return m_renderer;
}

//-----------------------------------------------------------------------------

void FrameBufferItem::initialize()
{
    std::cout << "Initialize" << std::endl;
    m_win->SetSize(width(), height());
    m_interactor->SetSize(m_win->GetSize());
    m_win->OpenGLInit();
}

//-----------------------------------------------------------------------------

bool FrameBufferItem::event(QEvent* evt)
{
    switch (evt->type())
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
            break;
        case QEvent::Resize:
            break;

        default:
            if (this->m_win && this->m_win->GetInteractor())
            {
                lockRenderer();
                this->m_interactorAdapter->ProcessEvent(evt, this->m_win->GetInteractor());
                unlockRenderer();
                if (evt->isAccepted())
                {
                    return true;
                }
            }
    }
    return QQuickFramebufferObject::event(evt);
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mousePressEvent(QMouseEvent* event)
{
    if (this->m_win && this->m_win->GetInteractor())
    {
        lockRenderer();
        this->m_interactorAdapter->ProcessEvent(event, this->m_win->GetInteractor());
        unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseMoveEvent(QMouseEvent* event)
{
    if (this->m_win && this->m_win->GetInteractor())
    {
        lockRenderer();
        this->m_interactorAdapter->ProcessEvent(event, this->m_win->GetInteractor());
        unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (this->m_win && this->m_win->GetInteractor())
    {
        lockRenderer();
        this->m_interactorAdapter->ProcessEvent(event, this->m_win->GetInteractor());
        unlockRenderer();
    }
}

//-----------------------------------------------------------------------------

void FrameBufferItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (this->m_win && this->m_win->GetInteractor())
    {
        lockRenderer();
        this->m_interactorAdapter->ProcessEvent(event, this->m_win->GetInteractor());
        unlockRenderer();
    }
}

//------------------------------------------------------------------------------

void FrameBufferItem::lockRenderer()
{
    m_viewLock.lock();
}

//------------------------------------------------------------------------------

void FrameBufferItem::unlockRenderer()
{
    m_viewLock.unlock();
}

} // fwRenderVTK
