/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwPacsIO/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwMemory/BufferObject.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <filesystem>

namespace fwPacsIO
{

/**
 * @brief Reads DICOM series from pacs.
 */
class FWPACSIO_CLASS_API SeriesEnquirer : public ::fwCore::BaseObject,
                                          protected DcmSCU
{

public:
    fwCoreClassMacro(SeriesEnquirer, ::fwPacsIO::SeriesEnquirer, new SeriesEnquirer);
    fwCoreAllowSharedFromThis();

    FWPACSIO_API static const ::fwCom::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;
    typedef ::fwCom::Slot<void (const std::string&, unsigned int, const std::string&)> ProgressCallbackSlotType;

    typedef std::vector< std::string > InstanceUIDContainer;

    typedef std::vector< std::filesystem::path > InstancePathContainer;

    typedef std::vector< CSPTR(DcmDataset) > DatasetContainer;

    /// Constructor
    FWPACSIO_API SeriesEnquirer();

    /// Destructor
    FWPACSIO_API ~SeriesEnquirer();

    /**
     * @brief Initialize the connection
     * @param[in] applicationTitle Local application title
     * @param[in] peerHostName Pacs host name
     * @param[in] peerPort Pacs port
     * @param[in] peerApplicationTitle Pacs application title
     * @param[in] path Local dicom folder path
     * @param[in] moveApplicationTitle Move application title
     * @param[in] progressCallback Progress callback
     */
    FWPACSIO_API void initialize(const std::string& applicationTitle,
                                 const std::string& peerHostName, unsigned short peerPort,
                                 const std::string& peerApplicationTitle,
                                 const std::string& moveApplicationTitle         = "",
                                 ProgressCallbackSlotType::sptr progressCallback = ProgressCallbackSlotType::sptr());

    /// Negotiate Association
    FWPACSIO_API bool connect();

    /// Release association
    FWPACSIO_API void disconnect();

    /// Assemble and send C-ECHO request
    FWPACSIO_API bool pingPacs();

    /// Return true if there is an existing association
    FWPACSIO_API bool isConnectedToPacs() const;

    /**
     * @brief Find series by patient name
     * @param[in] name Patient name
     * @return dcmtk response list
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByPatientName(const std::string& name);

    /**
     * @brief Find series by study date
     * @param[in] fromDate Beginning date
     * @param[in] toDate Ending date
     * @return dcmtk response list
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByDate(const std::string& fromDate, const std::string& toDate);

    /**
     * @brief Find series by series UID
     * @param[in] uid Series UID
     * @return dcmtk response list
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByUID(const std::string& uid);

    /**
     * @brief Find SOPInstanceUID of the specified instance
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @return SOPInstanceUID corresponding to the instanceNumber
     */
    FWPACSIO_API std::string findSOPInstanceUID(const std::string& seriesInstanceUID, unsigned int instanceNumber);

    /**
     * @brief Pull series using C-MOVE requests
     * @param[in] instanceUIDContainer Series instance UID container
     */
    FWPACSIO_API void pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer instanceUIDContainer);

    /**
     * @brief Pull series using C-GET requests
     * @param[in] instanceUIDContainer Series instance UID container
     */
    FWPACSIO_API void pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer instanceUIDContainer);

    /**
     * @brief Pull instance using C-MOVE requests
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] sopInstanceUID SOP Instance UID
     */
    FWPACSIO_API void pullInstanceUsingMoveRetrieveMethod(const std::string& seriesInstanceUID,
                                                          const std::string& sopInstanceUID);

    /**
     * @brief Pull instance using C-GET requests
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] sopInstanceUID SOP Instance UID
     */
    FWPACSIO_API void pullInstanceUsingGetRetrieveMethod(const std::string& seriesInstanceUID,
                                                         const std::string& sopInstanceUID);

    /**
     * @brief Push instances using C-STORE requests
     * @param[in] pathContainer Instance paths container
     */
    FWPACSIO_API void pushSeries(const InstancePathContainer& pathContainer);

    /**
     * @brief Push instances using C-STORE requests
     * @param[in] DatasetContainer DICOM dataset container
     */
    FWPACSIO_API void pushSeries(const DatasetContainer& datasetContainer);

protected:

    /**
     * @brief Send Find Request
     * @param[in] dataset Request dataset
     * @return dcmtk response list
     */
    FWPACSIO_API OFList< QRResponse* > sendFindRequest(DcmDataset dataset);

    /**
     * @brief Send Move Request
     * @param[in] dataset Request dataset
     * @return OFTrue on success
     */
    FWPACSIO_API OFCondition sendMoveRequest(DcmDataset dataset);

    /**
     * @brief Send Get Request
     * @param[in] dataset Request dataset
     * @return OFTrue on success
     */
    FWPACSIO_API OFCondition sendGetRequest(DcmDataset dataset);

    /**
     * @brief Send Store Request
     * @param[in] path File path
     * @return OFTrue on success
     */
    FWPACSIO_API OFCondition sendStoreRequest(const std::filesystem::path& path);

    /**
     * @brief Send Store Request
     * @param[in] dataset Dicom dataset
     * @return OFTrue on success
     */
    FWPACSIO_API OFCondition sendStoreRequest(const CSPTR(DcmDataset)& dataset);

    /// Handle MOVE Response (Override)
    FWPACSIO_API virtual OFCondition handleMOVEResponse(
        const T_ASC_PresentationContextID presID, RetrieveResponse* response, OFBool& waitForNextResponse) override;

    /// Handle STORE Request (Override)
    FWPACSIO_API virtual OFCondition handleSTORERequest (
        const T_ASC_PresentationContextID presID, DcmDataset* incomingObject,
        OFBool& continueCGETSession, Uint16& cStoreReturnStatus) override;

    /**
     * @brief Find uncompressed presentation context
     * @param[in] sopClass SOP class which needs the presentation context
     */
    FWPACSIO_API Uint8 findUncompressedPC(const OFString& sopClass);

    /// MOVE destination AE Title
    std::string m_moveApplicationTitle;

    /// Path where the files must be saved
    std::filesystem::path m_path;

    /// Progress callback slot
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Dowloaded instance index
    unsigned int m_instanceIndex;
};

} // namespace fwPacsIO
