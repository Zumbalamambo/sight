/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__
#define __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__

#include <vector>

#include <fwComEd/InteractionMsg.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PickerInteractor : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PickerInteractor)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API PickerInteractor() throw();

    VISUVTKADAPTOR_API virtual ~PickerInteractor() throw();

protected:
    friend class PickerInteractorCallback;

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
    {
    }

    VISUVTKADAPTOR_API void notifyEvent(::fwComEd::InteractionMsg::sptr msg);



    vtkCommand *m_interactionCommand;
    float m_priority;

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_PICKERINTERACTOR_HPP__
