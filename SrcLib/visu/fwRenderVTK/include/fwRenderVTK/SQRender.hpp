#ifndef SQRENDER_HPP
#define SQRENDER_HPP

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/FrameBufferItem.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/HasSignals.hpp>
#include <fwCom/HasSlots.hpp>

#include <fwRender/IRender.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/helper/Config.hpp>
#include <fwServices/IQmlService.hpp>

#include <fwServices/QtObjectHolder.hpp>

#include <QVariantMap>
#include <QMap>
#include <QVector>

#include <map>

class vtkRenderWindow;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkObject;
class vtkTransform;

// Forward declarations
namespace fwData
{
class TransformationMatrix3D;
}

namespace fwThread
{
class Timer;
}

namespace fwRenderVTK
{

class IAdaptor;
class IVtkRenderWindowInteractorManager;

/**
 * @brief The generic scene service shows adaptors in a 3D VTK scene.
 *
 * @section Signals Signals
 * - \b dropped(std::string) : emitted when an object is dropped in the scene (contains the uid of the object)
 *
 * @section Slots Slots
 * - \b render() : render the scene
 * - \b requestRender() : request a render of the scene (call to render only if the scene is displayed)
 * - \b toggleAutoRender() : toggle the render mode between AUTO and NONE
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="generiSceneUID" impl="::fwRenderVTK::SQRender" type="::fwRender::IRender">
        <scene renderMode="auto|timer|none" width="1920" height="1080">
            <renderer id="background" layer="0" background="0.0" />
            <vtkObject id="transform" class="vtkTransform" />
            <picker id="negatodefault" vtkclass="fwVtkCellPicker" tolerance="0.002"/>

            <adaptor uid="tmAdaptor" />
            <adaptor uid="snapshot" />
        </scene>
        <fps>30</fps>
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b offScreen [::fwData::Image] (optional, unused by default): If used, render the scene in an image
 * and not in a window. You can add a <flip>true</flip> tag if you want to flip the off screen image result.
 *
 * @subsection Configuration Configuration
 * - \b renderMode (optional, "auto" by default): this attribute is forwarded to all adaptors. For each adaptor,
 *   if renderMode="auto",  the scene is automatically rendered after doStart, doUpdate, doSwap, doStop
 *   and m_vtkPipelineModified=true. If renderMode="timer" the scene is rendered at N frame per seconds (N is
 *   defined by fps tag). If renderMode="none" you should call 'render' slot to call reder the scene.
 * - \b width (optional, "1280" by default): width for off screen render
 * - \b height (optional, "720" by default): height for off screen render
 * - \b renderer
 *    - \b id (mandatory): the identifier of the renderer
 *    - \b layer (optional): defines the layer of the vtkRenderer. This is only used if there are layered
 * enderers.
 *    - \b background (optional): the background color of the rendering screen. The color value can be defines as a
 *      grey level value (ex . 1.0 for white) or as a hexadecimal value (ex : \#ffffff for white).
 * - \b vtkObject
 *   - \b id (mandatory): the identifier of the vtkObject
 *   - \b class (mandatory): the classname of the vtkObject to create. For example vtkTransform, vtkImageBlend, ...
 * - \b picker
 *   - \b id (mandatory): the identifier of the picker
 *   - \b vtkclass (optional, by default vtkCellPicker): the classname of the picker to create.
 *   - \b tolerance (optional, by default 0.0): the picker tolerance specified as fraction of rendering window size.
 * - \b adaptor
 *   - \b uid (mandatory): the uid of the adaptor
 * - \b flip (optional): if true, flip the output of the renderer in offscreen mode.
 */
class FWRENDERVTK_CLASS_API SQRender : public ::fwServices::IQmlService
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap scene READ scene WRITE setScene)
    Q_PROPERTY(unsigned int width MEMBER m_width)
    Q_PROPERTY(unsigned int height MEMBER m_height)
    Q_PROPERTY(bool offScreen MEMBER m_offScreen)
    Q_PROPERTY(bool flip MEMBER m_flip)
    Q_PROPERTY(unsigned int targetFrameRate MEMBER m_targetFrameRate)
    Q_PROPERTY(fwServices::QtObjectHolder *inout MEMBER m_inout)
    Q_PROPERTY(FrameBufferItem *target MEMBER m_target)


public:

    typedef std::string RendererIdType;
    typedef std::string PickerIdType;
    typedef std::string ObjectIdType;
    typedef std::string AdaptorIdType;
    typedef std::string VtkObjectIdType;
    typedef std::map< std::string, ::fwData::Object::csptr > ConstObjectMapType;
    using ConfigType = QVariantMap;

    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_RENDER_SLOT;
    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_REQUEST_RENDER_SLOT;
    FWRENDERVTK_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_AUTO_RENDER_SLOT;

    FWRENDERVTK_API static const ::fwCom::Signals::SignalKeyType s_DROPPED_SIG;
    typedef ::fwCom::Signal< void (std::string)> DroppedSignalType;

    FWRENDERVTK_API SQRender() noexcept;

    FWRENDERVTK_API virtual ~SQRender() noexcept;

    /// Returns the vtkRenderer with the given id
    FWRENDERVTK_API vtkRenderer* getRenderer(RendererIdType rendererId);

    /// Returns true if the scene is shown on screen
    bool isShownOnScreen();

    /// Returns the picker with the given id
    FWRENDERVTK_API vtkAbstractPropPicker* getPicker(PickerIdType pickerId);

    /// Returns the vtkObject with the given id
    FWRENDERVTK_API vtkObject* getVtkObject(const VtkObjectIdType& objectId) const;

    /// Get a vtkTransform in the SQRender, referenced by a key. Create it if it does not exist.
    FWRENDERVTK_API vtkTransform* getOrAddVtkTransform( const VtkObjectIdType& _id );

    bool getPendingRenderRequest() const;

    FrameBufferItem *getTarget() const;

    void setPendingRenderRequest(bool b);

    enum class RenderMode
    {
        NONE,
        AUTO,
        TIMER
    };

    /// Gets the render mode
    RenderMode getRenderMode() const;

    /// Tells if the rendering is done offscreen
    FWRENDERVTK_API bool isOffScreen() const;

    /// Allows to change the size of the offscreen renderer at runtime. This overrides XML configuration settings.
    FWRENDERVTK_API void setOffScreenRenderSize(unsigned int _width, unsigned int _height);

    FWRENDERVTK_API QVariantMap const&  scene() const;
    FWRENDERVTK_API void    setScene(QVariantMap const& scene);

protected:

    /// Renders the scene.
    FWRENDERVTK_API void render();

    /// Starts the render context, start the adaptors with start="yes"
    FWRENDERVTK_API virtual void starting() override;

    /// Stops the context, stops the started adaptors
    FWRENDERVTK_API virtual void stopping() override;

    /// configures the scene
    FWRENDERVTK_API virtual void configuring() override;

    /// Does nothing.
    FWRENDERVTK_API virtual void updating() override;

    /// Add a vtk object in the SQRender, referenced by a key.
    FWRENDERVTK_API void addVtkObject( const VtkObjectIdType& _id, vtkObject* _vtkObj );

private:



    /// Slot: called when on each timer update
    void requestRender();

    /// Slot: called to toggle the render mode between AUTO and NONE
    void toggleAutoRender();

    /// Installs scene interactors
    void startContext();

    /// Remove scene interactors
    void stopContext();

    /// Parse the 'picker' configuration and create the associated vtk picker
    void configurePicker   (const ConfigType& pickerConf );

    /// Parse the 'renderer' configuration and create the associated vtk renderer
    void configureRenderer (const ConfigType& rendererConf );

    /// Parse the 'vtkObject' configuration and create the associated vtk objects
    void configureVtkObject(const ConfigType& vtkObjectConf );

    /// Parse the transform configuration to create vtk transform and manage concatenate
    vtkTransform* createVtkTransform(const ConfigType& vtkObjectConf );

    /// @brief VTK Interactor window manager
    SPTR( ::fwRenderVTK::IVtkRenderWindowInteractorManager ) m_interactorManager;

    /// @brief scene's renderers
    typedef std::map< RendererIdType, vtkRenderer* > RenderersMapType;

    /// @brief scene's pickers
    typedef std::map< PickerIdType, vtkAbstractPropPicker* > PickersMapType;

    /// @brief scene's transforms
    typedef std::map< VtkObjectIdType, vtkObject* > VtkObjectMapType;

    RenderersMapType m_renderers;
    PickersMapType m_pickers;
    VtkObjectMapType m_vtkObjects;

    bool m_pendingRenderRequest;

    /// Does the scene update automatically when something changes ?
    /// Otherwise it is updated periodically (default 30Hz)
    RenderMode m_renderMode;

    /// Timer used for the update
    SPTR( ::fwThread::Timer ) m_timer;

    /// Stores the scene connfiguration.
    ConfigType m_sceneConf;

    /**
     *  Qml variable configuration exposure
     *  @{
     */
    QVariantMap m_qSceneConf;
    unsigned int m_width; ///< width for off screen render
    unsigned int m_height; ///< height for off screen render
    unsigned int m_targetFrameRate;
    bool m_offScreen; ///< if true, scene is render in off screen
    bool m_flip; ///< if true, flip off screen render scene
    ::fwServices::QtObjectHolder *m_inout;
    FrameBufferItem *m_target;
    /**
     * }@
     */
};

//-----------------------------------------------------------------------------

inline bool SQRender::getPendingRenderRequest() const
{
    return m_pendingRenderRequest;
}

//------------------------------------------------------------------------------

inline void SQRender::setPendingRenderRequest(bool b)
{
    m_pendingRenderRequest = b;
}

//-----------------------------------------------------------------------------

inline SQRender::RenderMode SQRender::getRenderMode() const
{
    return m_renderMode;
}

}


#endif // SQRENDER_HPP
