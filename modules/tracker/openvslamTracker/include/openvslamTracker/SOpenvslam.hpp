/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "openvslamTracker/config.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>

#include <arServices/ITracker.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/HiResTimer.hpp>

#include <fwData/Float.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>

#include <fwThread/Timer.hpp>

#include <openvslamIO/OpenvslamConfig.hpp>

#include <opencv2/opencv.hpp>

namespace openvslam
{
class system;
class config;

namespace publish
{
class map_publisher;
class frame_publisher;
}
}

/// The module openvslamTracker contains SOpenvslam service to manage instance of OpenVSLAM.
namespace openvslamTracker
{

/**
 * @brief Service that manages an instance of Openvslam (Simultaneous tracking and Mapping).
 *
 * It will populate a matrixTL with camera position, and can also export the map as a pointcloud (::fwData::Mesh).
 * Note: in this version of the service only monocular (MONO) mode is available, and the downsample feature is disabled.
 *
 * @section Signals Signals
 * - \b trackingInitialized(): Emitted when the tracking is initialized.
 * - \b trackingNotInitialized(): Emitted when the tracking must be reinitialized.
 * - \b tracked(): Emitted when openvslam is tracking.
 * - \b trackingLost(): Emitted when tracking is lost.
 * - \b vocFileUnloaded(): Emitted when the vocabulary file is unloaded.
 * - \b vocFileLoadingStarted(): Emitted when the vocabulary file is loading.
 * - \b vocFileLoaded(): Emitted when the vocabulary file is loaded.
 *
 * @section Slots Slots
 * - \b startTracking(): Initialize and start the tracking process.
 * - \b stopTracking(): Stop the tracking process.
 * - \b track(timestamp): Call openvslam with the new frame.
 * - \b enableLocalization(bool): Enable the localization mode by stopping mapping thread.
 * - \b activateLocalization(): Activate the localization mode by stopping mapping thread.
 * - \b resetPointCloud(): Reset the pointcloud.
 * - \b saveMap(): Save openvslam's map.
 * - \b loadMap(): Load openvslam's map.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *   - scaleFactor: to rescale matrix and points (by default Monocular use an arbitrary scale).
 *   - initializer.parallaxDegThr: Parallax threshold in degree for openvslam initializer (advanced).
 *   - initializer.reprojErrThr: Reprojection error threshold for openvslam initializer (advanced).
 *   - initializer.scalingFactor: Initial scale magnitude for openvslam initializer (advanced).
 * - \b setIntParameter(int, string): Calls an int parameter slot according to the given key.
 *   - nFeatures: Set the number of features per image.
 *   - nLevels: Set the number of levels in the scale pyramid.
 *   - iniThFAST: Initial FAST Threshold value .
 *   - minThFAST: Min FAST Threshold value.
 *   - initializer.numRansacIterations: Number of RANSAC iteration of openvslam initializer (advanced).
 *   - initializer.minNumTriangulatedPts: Minimal number of triangulated points for openvslam initializer (advanced).
 *   - initializer.numBAIterations: Number of iterations of the Bundle-Adjustement for openvslam initializer (advanced).
 * - \b setBoolParameter(bool, string): Calls a bool parameter slot according to the given key.
 *   - showFeatures: Call an imshow to display internal image of openvslam (with features projected).
 * - \b setEnumParameter(string, string): Calls an enum parameter slot according to the given key.
 * - MapType: "Global" or "Local", fill either the global map or the local map in the pointcloud output.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::openvslamTracker::SOpenvslam" worker="trackerWorker" >
            <in key="camera" uid="..." />
            <in key="timeline" uid="..." autoConnect="yes" />
            <in key="timeline2" uid="..." />
            <inout key="cameraMatrixTL" uid="..." />
            <out key="pointCloud" uid="..." />
            <mode>MONO</mode>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera](mandatory): camera that will be tracked.
 * - \b timeline [::arData::FrameTL](mandatory): timeline of frames of the video on which openvslam will work.
 * - \b timeline2 [::arData::FrameTL](optional): Only needed if STEREO/DEPTH mode is enabled !
 * if STEREO: frameTL2 will represent frame from the second camera.
 * if DEPTH: frameTL2 will represent frame from depth sensor.
 *
 * @subsection In-Out In-Out:
 * - \b cameraMatrixTL [::arData::MatrixTL](optional): timeLine of  matrix representing the movement of the 3D
 * camera and thus of the real camera.
 *
 * @subsection Output Output:
 * - \b pointCloud [::fwData::Mesh](optional): mesh containing the 3D points given by openvslam
 *
 * @subsection Configuration Configuration:
 * - \b mode (optional): set the tracking mode (MONO, STEREO, DEPTH), by default MONO will be used.
 *  Note that if STEREO/DEPTH mode is used  the frameTL2 input will be needed.
 * - \b mapFile (optional): if specified, the service will attempt to load the specified map file when tracking is
 *   started and to save it when tracking is stopped. If this option is not specified or if the file is not found when
 *   starting the tracking, an empty map will be created instead.
 */
class OPENVSLAMTRACKER_CLASS_API SOpenvslam : public ::arServices::ITracker
{

public:

    fwCoreServiceMacro(SOpenvslam, ::arServices::ITracker)

    /// Constructor. Initializes signals and slots.
    OPENVSLAMTRACKER_API SOpenvslam() noexcept;

    /// Destructor. Stops the service if started.
    OPENVSLAMTRACKER_API virtual ~SOpenvslam() noexcept override final;

    /**
     * @name Tracking Mode : Openvslam can be used with 3 mode.
     * - \b MONO : Use only a monocular camera.
     * - \b STEREO: Use a sterovision system.(NOT IMPLEMENTED)
     * - \b DEPTH : Use a RGB-D sensor. (NOT IMPLEMENTED)
     */
    enum class OPENVSLAMTRACKER_API TrackingMode
    {
        MONO = 0,
        STEREO,
        DEPTH
    };

protected:

    /// Configures the service by parsing XML.
    OPENVSLAMTRACKER_API virtual void configuring() override final;

    /// Retrieves input data.
    OPENVSLAMTRACKER_API virtual void starting() override final;

    /// Shutdown the openvslam system & reset output.
    OPENVSLAMTRACKER_API virtual void stopping() override final;

    /// Does nothing.
    OPENVSLAMTRACKER_API virtual void updating() override final;

private:

    /**
     * @name Slots methods
     *
     * @{
     */
    /// Slot: called to start the tracking.
    virtual void startTracking() override final;

    /// Slot: called to stop the tracking.
    virtual void stopTracking() override final;

    /// Slot: called to enable/disable localization mode (stop/(re)start mapping thread).
    void enableLocalization(bool);

    /// Slot: called to enable localization mode.
    void activateLocalization();

    /// Slot: called to disable localization mode.
    void deactivateLocalization();

    /// Slot: called when a integer value is changed.
    void setIntParameter(int, std::string);

    /// Slot: called when a double value is changed.
    void setDoubleParameter(double, std::string);

    /// Slot: called when a bool value is changed.
    void setBoolParameter(bool, std::string);

    /// Slot: called when an enum value is changed.
    void setEnumParameter(std::string, std::string);

    /// Slot: Load Openvslam map file.
    void loadMap();

    /// Slot: Save Openvslam map file.
    void saveMap();

    /// Slot: Save trajectories files (both frame & KeyFrames).
    void saveTrajectories();

    /// Slot: Pause/resume tracker.
    void pause();

    /// Slot: called to reset the pointcloud.
    void resetPointCloud();

    /// Slot: call openvslam with the new frame.
    void virtual tracking(::fwCore::HiResClock::HiResClockType&) override final;
    /** @} */

    /**
     * @name Signals API
     * @{
     */
    /// Type of the signals.

    using SignalType = ::fwCom::Signal<void()>;
    /** @} */

    /**
     * @name Signals objects
     *
     * @{
     */
    /// Signal: sent when the tracking is initialized.
    SignalType::sptr m_sigTrackingInitialized;

    /// Signal: sended when the tracking is not initialized.
    SignalType::sptr m_sigTrackingNotInitialized;

    /// Signal: sent when the tracking is found.
    SignalType::sptr m_sigTracked;

    /// Signal: sent when the tracking is lost.
    SignalType::sptr m_sigTrackingLost;

    /// Signal: sent when the vocabulary file is unloaded.
    SignalType::sptr m_sigVocFileUnloaded;

    /// Signal: sent when the vocabulary file is loading.
    SignalType::sptr m_sigVocFileLoadingStarted;

    /// Signal: sent when the vocabulary file is loaded.
    SignalType::sptr m_sigVocFileLoaded;

    /// Signal: sent when the map file is loaded.
    SignalType::sptr m_sigMapLoaded;
    /** @} */

    /// Start the tracking with the path of the _mapFile.
    void startTracking(const std::string& _mapFile);

    /// Load Vocabulary file using the path _mapFile.
    void loadVocabulary(const std::string& _mapfile);

    /// Update pointcloud from openvslam's map.
    void updatePointCloud();

private:

    /// Transformation matrix representing the movement of the 3D camera and thus of the real camera.
    ::arData::MatrixTL::sptr m_cameraMatrixTL;

    /// Timeline of frames of the video on which openvslam will work.
    ::arData::FrameTL::csptr m_frameTL;

    /// Timeline of frames of the second video (STEREO/DEPTH mode), unused in MONO mode.
    ::arData::FrameTL::csptr m_frameTL2;

    /// Virtual scene camera.
    ::arData::Camera::csptr m_camera;

    /// Mesh that represent MapPoints.
    ::fwData::Mesh::sptr m_pointCloud;

    /// ORB Parameters structure
    ::openvslamIO::OrbParams m_orbParameters;

    /// Openvslam initializer parameters (only used in monocular mode).
    ::openvslamIO::InitParams m_initializerParameters;

    /// Tracking mode : MONO, STEREO, DEPTH.
    TrackingMode m_trackingMode {TrackingMode::MONO};

    /// Target width when downsampling is required.
    size_t m_downSampleWidth {0};

    /// Mutex to lock m_slamSystem.
    std::mutex m_slamLock;

    /// If localization mode is enable
    bool m_localization {false};

    /// Unique pointer to SLAM system.
    std::unique_ptr< openvslam::system > m_slamSystem {nullptr};

    /// Pointer to a publisher class to get current frame.
    std::shared_ptr< ::openvslam::publish::frame_publisher> m_ovsFramePublisher {nullptr};
    /// Pointer to a publisher class to get current map.
    std::shared_ptr< ::openvslam::publish::map_publisher> m_ovsMapPublisher {nullptr};

    /// Path to the vocabulary (could be set only once).
    std::string m_vocabularyPath;

    /// To show detected features in an external view (using cv::imshow).
    bool m_showFeatures {false};

    /// Switch between local or global map (pushed in m_pointcloud).
    bool m_localMap{false};

    ///Calls asynchronously updatePointCloud each 1sec.
    ::fwThread::Timer::sptr m_timer;

    /// Matrix and points scale
    std::atomic< float > m_scale{1.0f};

    /// Worker for pointcloud update
    ::fwThread::Worker::sptr m_pointcloudWorker;

    /// Pause state.
    bool m_isPaused {false};

    /// Stores the filepath to save map.
    std::string m_saveMapPath;

    /// Stores the folder where to save trajectories files.
    ::fwData::location::SingleFile::sptr m_trajectoriesSavePath;

    /// Stores the trajectories format ("KITTI" or "TUM" are internal formats in openvslam).
    /// This is only used when saving trajectories at stop.
    /// KITTI = matrices , TUM = vectors & quaternions.
    std::string m_trajectoriesFormat{"KITTI"};

    /// Stores the current number of landmarks in the map. (Only used in updatePointCloud thread).
    unsigned int m_numberOfLandmarks {0};

};

} // openvslamTracker
