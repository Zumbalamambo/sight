/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <fwRenderOgre/config.hpp>
#include <fwRenderOgre/factory/new.hpp>
#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/picker/IPicker.hpp>
#include <fwRenderOgre/registry/detail.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwData/Object.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{
namespace interactor
{

/**
 * @brief Interface implementation for all selection with the mouse
 *
 * @deprecated removed in sight 21.0
 */
class FWRENDEROGRE_CLASS_API IPickerInteractor : public ::fwRenderOgre::interactor::IInteractor,
                                                 public ::fwCore::BaseObject,
                                                 public ::fwCom::HasSignals
{

public:

    fwCoreClassMacro(IPickerInteractor, ::fwCore::BaseObject)

    /**
     * @brief Class used to register a class factory in factory registry. This class defines also the object factory (
     *'create' ).
     *
     * @tparam T Factory product type.
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwRenderOgre::registry::getInteractorRegistry()->addFactory(functorKey,
                                                                          &::fwRenderOgre::interactorFactory::New<T>);
        }
    };

    /// Used to send picking info.
    typedef ::fwCom::Signal< void ( ::fwDataTools::PickingInfo ) > PointClickedSigType;
    FWRENDEROGRE_API static const ::fwCom::Signals::SignalKeyType s_PICKED_SIG;

    /// Retrieves the Ogre root and the \<sceneID\> scene manager.
    [[deprecated("Class removed in sight 21.0")]]
    FWRENDEROGRE_API IPickerInteractor();

    /// Delete resources.
    [[deprecated("Class removed in sight 21.0")]]
    FWRENDEROGRE_API virtual ~IPickerInteractor();

    /// Initialize picker with the corresponding layer's info.
    FWRENDEROGRE_API void initPicker();

    /// Return true if the picker is initialized.
    FWRENDEROGRE_API bool isPickerInitialized() const;

    /**
     * @brief Set the query mask.
     * @param _queryMask The value of the query mask.
     */
    FWRENDEROGRE_API void setQueryMask(std::uint32_t _queryMask);

protected:

    /// Signal triggered when an action has been triggered.
    PointClickedSigType::sptr m_picked {nullptr};

    /// Signal triggered when a render is requested.
    RenderRequestedSigType::sptr m_sigRenderRequested {nullptr};

    /// Ogre picker.
    ::fwRenderOgre::picker::IPicker* m_picker {nullptr};

    /// Mask for picking requests.
    std::uint32_t m_queryMask {::Ogre::SceneManager::ENTITY_TYPE_MASK};
};

}
}
