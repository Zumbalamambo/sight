/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOCALIBRATION_SREPROJECTIONERROR_HPP__
#define __VIDEOCALIBRATION_SREPROJECTIONERROR_HPP__

#include "videoCalibration/config.hpp"

#include <fwCore/HiResClock.hpp>

#include <fwServices/IController.hpp>

#include <opencv2/core.hpp>

namespace videoCalibration
{
/**
 * @brief   SReprojectionError Class used to compute the mean error of reprojection between 3d object points
 * and 2d image points.
 *
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="..." type="::tracker::SReprojectionError">
        <in key="matrixTL" uid="matrix"/>
        <in key="markerTL" uid="marker"/>
        <in key="camera" uid="camera1"/>
        <in key="extrinsic" uid="matrix"/>
        <out key="error" uid="doubleError" />
        <patternWidth>80</patternWidth>
     </service>
   @endcode
 * @subsection Input Input
 * - \b markerTL [::arData::MarkerTL]: timeline for markers.
 * - \b camera [::arData::Camera]: calibrated cameras.
 * - \b extrinsic [::fwData::TransformationMatrix3D]: extrinsic matrix, only used if you have two cameras configured.
 * - \b matrixTL [::arData::MatrixTL]: timeline of 3D transformation matrices.
 * @subsection Output Output
 * - \b error [::fwData::Float] : computed error
 * @subsection Configuration Configuration
 * - \b patternWidth : width of the tag.
 */
class VIDEOCALIBRATION_CLASS_API SReprojectionError : public ::fwServices::IController
{
public:
    fwCoreServiceClassDefinitionsMacro((SReprojectionError)(fwServices::IController));

    /// Double changed signal type
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;

    static const ::fwCom::Slots::SlotKeyType s_COMPUTE_SLOT;

    ///Constructor
    VIDEOCALIBRATION_API SReprojectionError();

    ///Destructor
    VIDEOCALIBRATION_API ~SReprojectionError();

    /// Connect MatrixTL::s_OBJECT_PUSHED_SIG to s_COMPUTE_SLOT
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:
    /**
     * @brief Configuring method : This method is used to configure the service.
     */
    VIDEOCALIBRATION_API void configuring() throw (fwTools::Failed);

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    VIDEOCALIBRATION_API void starting() throw (fwTools::Failed);

    /**
     * @brief Updating method : This method is used to update the service.
     */
    VIDEOCALIBRATION_API void updating() throw (fwTools::Failed);

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    VIDEOCALIBRATION_API void stopping() throw (fwTools::Failed);

private:

    void compute(::fwCore::HiResClock::HiResClockType timestamp);

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

    /// Marker pattern width.
    double m_patternWidth;

    ///
    std::vector< ::cv::Point3f > m_objectPoints;

    ::cv::Mat m_cameraMatrix;
    ::cv::Mat m_distorsionCoef;

    bool m_display;

};

}//namespace videoCalibration

#endif //__VIDEOCALIBRATION_SREPROJECTIONERROR_HPP__
