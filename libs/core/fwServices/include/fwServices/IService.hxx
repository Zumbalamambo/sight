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

#pragma once

#include "fwServices/IService.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

template< class DATATYPE >
inline CSPTR(DATATYPE) IService::getInput(const KeyType& key) const
{
    CSPTR(DATATYPE) input;

    auto iterator = m_inputsMap.find(key);
    if(iterator != m_inputsMap.end())
    {
        input = std::dynamic_pointer_cast<const DATATYPE>( iterator->second.getShared() );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", input);
    }

    return input;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline SPTR(DATATYPE) IService::getInOut(const KeyType& key) const
{
    SPTR(DATATYPE) inout;
    auto iterator = m_inOutsMap.find(key);
    if(iterator != m_inOutsMap.end())
    {
        inout = std::dynamic_pointer_cast<DATATYPE>( iterator->second.getShared() );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", inout);
    }

    return inout;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline SPTR(DATATYPE) IService::getOutput(const KeyType& key) const
{
    SPTR(DATATYPE) output;
    auto iterator = m_outputsMap.find(key);
    if(iterator != m_outputsMap.end())
    {
        output = std::dynamic_pointer_cast<DATATYPE>( iterator->second.getShared() );
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler<DATATYPE>().getClassname() << " failed", output);
    }

    return output;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline CSPTR(DATATYPE) IService::getInput(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getInput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline SPTR(DATATYPE) IService::getInOut(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getInOut< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}
//------------------------------------------------------------------------------

template< class DATATYPE >
inline SPTR(DATATYPE) IService::getOutput(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getOutput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE, typename CONST_DATATYPE >
inline ::fwData::mt::weak_ptr< CONST_DATATYPE > IService::getWeakInput(const KeyType& key) const
{
    ::fwData::mt::weak_ptr< CONST_DATATYPE > input;

    // Search the weak_ptr in the inputs map
    auto iterator = m_inputsMap.find(key);
    if(iterator != m_inputsMap.end())
    {
        // The Key has been found, we can cast it to the right type
        input = iterator->second.dynamicPointerCast< CONST_DATATYPE >();
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler< DATATYPE >().getClassname() << " failed",
                    !input.expired());
    }

    return input;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::weak_ptr< DATATYPE > IService::getWeakInOut(const KeyType& key) const
{
    ::fwData::mt::weak_ptr< DATATYPE > inout;

    // Search the weak_ptr in the inOuts map
    auto iterator = m_inOutsMap.find(key);
    if(iterator != m_inOutsMap.end())
    {
        // The Key has been found, we can cast it to the right type
        inout = iterator->second.dynamicPointerCast< DATATYPE >();
        OSLM_ASSERT("DynamicCast " << ::fwCore::TypeDemangler< DATATYPE >().getClassname() << " failed",
                    !inout.expired());
    }

    return inout;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::weak_ptr< DATATYPE > IService::getWeakOutput(const KeyType& key) const
{
    ::fwData::mt::weak_ptr< DATATYPE > output;

    // Search the weak_ptr in the inOuts map
    auto iterator = m_outputsMap.find(key);
    if(iterator != m_outputsMap.end())
    {
        // The Key has been found, we can cast it to the right type
        output = std::dynamic_pointer_cast< DATATYPE >(iterator->second.getShared());
        OSLM_ASSERT(
            "DynamicCast " << ::fwCore::TypeDemangler< DATATYPE >().getClassname() << " failed", !output.expired());
    }

    return output;
}

//------------------------------------------------------------------------------

template< class DATATYPE, typename CONST_DATATYPE >
inline ::fwData::mt::weak_ptr< CONST_DATATYPE > IService::getWeakInput(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getWeakInput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::weak_ptr< DATATYPE > IService::getWeakInOut(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getWeakInOut< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::weak_ptr< DATATYPE > IService::getWeakOutput(const KeyType& keybase, size_t index) const
{
#ifdef _DEBUG
    auto it = m_keyGroupSize.find(keybase);
#endif
    SLM_ASSERT("Key group '" + keybase + "' not found", it != m_keyGroupSize.end());
    OSLM_ASSERT("Index overflow '" << index << " >= " << it->second << "' in key group '" << keybase << ".",
                index < it->second);
    return this->getWeakOutput< DATATYPE >(KEY_GROUP_NAME(keybase, index));
}

//------------------------------------------------------------------------------

template< class DATATYPE, typename CONST_DATATYPE >
inline ::fwData::mt::locked_ptr< CONST_DATATYPE > IService::getLockedInput(const KeyType& key) const
{
    auto lockedInput = getWeakInput< DATATYPE >(key).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + key + "'.", lockedInput);
    return lockedInput;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::locked_ptr< DATATYPE > IService::getLockedInOut(const KeyType& key) const
{
    auto lockedInOut = getWeakInOut< DATATYPE >(key).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + key + "'.", lockedInOut);
    return lockedInOut;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::locked_ptr< DATATYPE > IService::getLockedOutput(const KeyType& key) const
{
    auto lockedOutput = getWeakOutput< DATATYPE >(key).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + key + "'.", lockedOutput);
    return lockedOutput;
}

//------------------------------------------------------------------------------

template< class DATATYPE, typename CONST_DATATYPE >
inline ::fwData::mt::locked_ptr< CONST_DATATYPE > IService::getLockedInput(const KeyType& keybase,
                                                                           size_t index) const
{
    auto lockedInput = getWeakInput< DATATYPE >(keybase, index).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + keybase + "'[" + std::to_string(index) + "].", lockedInput);
    return lockedInput;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::locked_ptr< DATATYPE > IService::getLockedInOut(const KeyType& keybase, size_t index) const
{
    auto lockedInOut = getWeakInOut< DATATYPE >(keybase, index).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + keybase + "'[" + std::to_string(index) + "].", lockedInOut);
    return lockedInOut;
}

//------------------------------------------------------------------------------

template< class DATATYPE >
inline ::fwData::mt::locked_ptr< DATATYPE > IService::geLockedOutput(const KeyType& keybase, size_t index) const
{
    auto lockedOutput = getWeakOutput< DATATYPE >(keybase, index).lock();
    SLM_ASSERT("Cannot acquire a lock on '" + keybase + "'[" + std::to_string(index) + "].", lockedOutput);
    return lockedOutput;
}

//------------------------------------------------------------------------------

inline size_t IService::getKeyGroupSize(const IService::KeyType& keybase) const
{
    auto it = m_keyGroupSize.find(keybase);
    if(it != m_keyGroupSize.end())
    {
        return it->second;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------

inline const IService::InputMapType& IService::getInputs() const
{
    return m_inputsMap;
}

//------------------------------------------------------------------------------

inline const IService::InOutMapType& IService::getInOuts() const
{
    return m_inOutsMap;
}

//------------------------------------------------------------------------------

inline const IService::OutputMapType& IService::getOutputs() const
{
    return m_outputsMap;
}

//------------------------------------------------------------------------------

} // namespace fwServices
