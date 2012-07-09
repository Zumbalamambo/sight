
#include "fwDataCamp/Factory.hpp"
#include "fwDataCamp/Camera.hpp"

fwCampImplementDataMacro((fwData)(Camera))
{
    builder.base< ::fwData::Object>()
        .tag("object_version", "2")
        .tag("lib_name", "fwData")
        .property("extrinsic_matric",&::fwData::Camera::m_extrinsicMatrix)
        .property("intrinsic_matrix",&::fwData::Camera::m_intrinsicMatrix)
        .property("data_available",&::fwData::Camera::m_dataAvailable)
        .property("skew",&::fwData::Camera::m_attrSkew)
        .property("distortion_coefficient",&::fwData::Camera::m_attrDistortionCoefficient)
        ;
}

