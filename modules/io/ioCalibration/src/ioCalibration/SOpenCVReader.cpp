/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "ioCalibration/SOpenCVReader.hpp"

#include <arData/CameraSeries.hpp>

#include <fwCom/Signal.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/dialog/LocationDialog.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwServices/macros.hpp>

#include <opencv2/core.hpp>

#include <sstream>

namespace ioCalibration
{

// ----------------------------------------------------------------------------

SOpenCVReader::SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

SOpenCVReader::~SOpenCVReader()
{
}

// ----------------------------------------------------------------------------

void SOpenCVReader::configuring()
{
    ::fwIO::IReader::configuring();
}

// ----------------------------------------------------------------------------

void SOpenCVReader::configureWithIHM()
{
    this->defineLocationGUI();
}

//----------------------------------------------------------------------------

bool SOpenCVReader::defineLocationGUI()
{
    bool ok = false;

    // Ask user for the file path
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Enter file name" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::SINGLE_FILE);
    dialogFile.addFilter("XML or YAML file", "*.xml *.yml *.yaml");

    ::fwData::location::SingleFile::sptr result
        = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );

    if (result)
    {
        _sDefaultPath = result->getPath();
        this->setFile( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath.parent_path()) );
        ok = true;
    }
    else
    {
        this->clearLocations();
    }

    return ok;
}

//----------------------------------------------------------------------------

void SOpenCVReader::starting()
{

}

// ----------------------------------------------------------------------------

void SOpenCVReader::stopping()
{

}

// ----------------------------------------------------------------------------

void SOpenCVReader::updating()
{

    ::arData::CameraSeries::sptr camSeries = this->getInOut< ::arData::CameraSeries >(::fwIO::s_DATA_KEY);

    bool use_dialog = false;
    //use dialog only if no file was configured
    if(!this->hasLocationDefined())
    {
        use_dialog = this->defineLocationGUI();
        if(!use_dialog)
        {
            this->m_readFailed = true;
            return;
        }
    }

    ::cv::FileStorage fs(this->getFile().string().c_str(), ::cv::FileStorage::READ); // Read the settings
    if (!fs.isOpened())
    {
        this->m_readFailed = true;
        SLM_ERROR("The file "+ this->getFile().string() + " cannot be opened.");
    }

    //Remove all CameraSeries
    // lock cameraSeries
    ::fwData::mt::ObjectReadToWriteLock lock(camSeries);
    const size_t cams = camSeries->getNumberOfCameras();

    for(size_t c = 0; c < cams; ++c)
    {
        ::arData::Camera::sptr cam = camSeries->getCamera(0);
        lock.upgrade();
        camSeries->removeCamera(cam);
        lock.downgrade();

        auto sig = camSeries->signal< ::arData::CameraSeries::RemovedCameraSignalType >
                       (::arData::CameraSeries::s_REMOVED_CAMERA_SIG);
        sig->asyncEmit(cam);

    }

    lock.unlock();

    int nbCameras;
    fs["nbCameras"] >> nbCameras;

    for(int c = 0; c < nbCameras; ++c)
    {
        std::stringstream camNum;
        camNum<<"camera_"<<c;

        ::cv::FileNode n = fs[camNum.str()];

        std::string id, desc;
        int width, height;
        ::cv::Mat matrix, dist;
        double scale;
        n["id"] >> id;
        n["description"] >> desc;
        n["imageWidth"] >> width;
        n["imageHeight"] >> height;
        n["matrix"] >> matrix;
        n["distortion"] >> dist;

        if(!n["scale"].empty())
        {
            n["scale"] >> scale;
        }
        else
        {
            scale = 1.;
        }

        ::arData::Camera::sptr cam = ::arData::Camera::New();
        cam->setFx(matrix.at<double>(0, 0));
        cam->setFy(matrix.at<double>(1, 1));
        cam->setCx(matrix.at<double>(0, 2));
        cam->setCy(matrix.at<double>(1, 2));

        cam->setCameraID(id);
        cam->setDescription(desc);
        cam->setWidth(static_cast<size_t>(width));
        cam->setHeight(static_cast<size_t>(height));

        cam->setDistortionCoefficient(dist.at<double>(0),
                                      dist.at<double>(1),
                                      dist.at<double>(2),
                                      dist.at<double>(3),
                                      dist.at<double>(4));

        cam->setScale(scale);
        cam->setIsCalibrated(true);

        ::fwData::mt::ObjectWriteLock writeLock(camSeries);
        camSeries->addCamera(cam);
        writeLock.unlock();

        auto sig = camSeries->signal< ::arData::CameraSeries::AddedCameraSignalType >(
            ::arData::CameraSeries::s_ADDED_CAMERA_SIG);
        sig->asyncEmit(cam);

        ::cv::Mat extrinsic;
        n["extrinsic"] >> extrinsic;

        if(!extrinsic.empty())
        {
            ::fwData::TransformationMatrix3D::sptr extMat = ::fwData::TransformationMatrix3D::New();

            for(size_t i = 0; i < 4; ++i)
            {
                for(size_t j = 0; j < 4; ++j)
                {
                    extMat->setCoefficient(i, j, extrinsic.at< double >(static_cast<int>(i),
                                                                        static_cast<int>(j)));
                }
            }
            writeLock.lock();
            camSeries->setExtrinsicMatrix(static_cast<size_t>(c), extMat);
            writeLock.unlock();
            auto sig = camSeries->signal< ::arData::CameraSeries::ExtrinsicCalibratedSignalType >
                           (::arData::CameraSeries::s_EXTRINSIC_CALIBRATED_SIG);
        }
    }

    fs.release(); // close file

    auto sig = camSeries->signal< ::arData::CameraSeries::ModifiedSignalType >(::arData::CameraSeries::s_MODIFIED_SIG);
    sig->asyncEmit();

    //clear locations only if it was configured through GUI.
    if(use_dialog)
    {
        this->clearLocations();
    }
}

// ----------------------------------------------------------------------------

void SOpenCVReader::swapping()
{
    this->stop();
    this->start();
}

// ----------------------------------------------------------------------------

::fwIO::IOPathType SOpenCVReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

// ----------------------------------------------------------------------------
} //namespace ioCalibration
