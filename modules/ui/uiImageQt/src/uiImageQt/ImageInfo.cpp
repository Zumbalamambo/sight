/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "uiImageQt/ImageInfo.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

namespace uiImageQt
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiImageQt::ImageInfo, ::fwData::Image )

static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";

ImageInfo::ImageInfo() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &ImageInfo::getInteraction, this);
}

//------------------------------------------------------------------------------

ImageInfo::~ImageInfo() noexcept
{
}

//------------------------------------------------------------------------------

void ImageInfo::starting()
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer() );

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText = new QLabel( QObject::tr("intensity:"));
    hLayout->addWidget( staticText, 0, Qt::AlignVCenter );

    m_valueText = new QLineEdit();
    m_valueText->setReadOnly(true);
    hLayout->addWidget( m_valueText, 1, Qt::AlignVCenter );

    qtContainer->setLayout( hLayout );
}

//------------------------------------------------------------------------------

void ImageInfo::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void ImageInfo::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void ImageInfo::updating()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    SLM_ASSERT("The input '" + s_IMAGE_INPUT + "' is not defined", image);
    const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
    m_valueText->setEnabled(imageIsValid);
}

//------------------------------------------------------------------------------

void ImageInfo::getInteraction(::fwDataTools::PickingInfo info)
{
    if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE)
    {
        ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
        SLM_ASSERT("The input '" + s_IMAGE_INPUT + "' is not defined", image);

        const bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );
        m_valueText->setEnabled(imageIsValid);
        if (imageIsValid)
        {
            const double* point              = info.m_worldPos;
            const ::fwData::Image::Size size = image->getSize2();

            if (point[0] < 0 || point[1] < 0 || point[2] < 0)
            {
                SLM_ERROR("The received coordinates are not in image space, maybe you used the wrong picker "
                          "interactor (see ::visuVTKAdaptor::SImagePickerInteractor)");
                return;
            }

            const ::fwData::Image::Size coords =
            {{ static_cast< ::fwData::Image::Size::value_type >(point[0]),
               static_cast< ::fwData::Image::Size::value_type >(point[1]),
               static_cast< ::fwData::Image::Size::value_type >(point[2])}};

            bool isInside = (coords[0] < size[0] && coords[1] < size[1]);
            if (image->getNumberOfDimensions() < 3)
            {
                isInside = (isInside && coords[2] == 0);
            }
            else
            {
                isInside = (isInside && coords[2] < size[2]);
            }

            if (!isInside)
            {
                SLM_ERROR("The received coordinates are not in image space, maybe you used the wrong picker "
                          "interactor (see ::visuVTKAdaptor::SImagePickerInteractor)");
                return;
            }

            const auto dumpLock = image->lock();

            const std::string intensity = image->getPixelAsString(coords[0], coords[1], coords[2] );
            m_valueText->setText(QString::fromStdString(intensity));
        }
    }
}

//------------------------------------------------------------------------------

void ImageInfo::info( std::ostream& _sstream )
{
    _sstream << "Image Info Editor";
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap ImageInfo::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------
}
