fwLoadProperties()

find_package(ITK QUIET REQUIRED)
find_package(JPEG QUIET REQUIRED)
find_package(ZLIB QUIET REQUIRED )

find_library(JPEG_LIBRARY libjpeg)

fwInclude(${ZLIB_INCLUDE_DIRS})

fwForwardInclude(
    ${ITK_INCLUDE_DIRS}
)

fwLink(
    ${ZLIB_LIBRARIES}
    ${JPEG_LIBRARY}
)

# Since ITK is template-based, this make sense to force the link for dependencies
fwForwardLink(
    ITKCommon
    ITKIOImageBase
    ITKIOJPEG
    ITKMetaIO
    itksys
    itkopenjpeg
)

