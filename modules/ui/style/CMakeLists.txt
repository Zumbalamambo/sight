fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Widgets REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Widgets)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}")

qt5_add_binary_resources(${FWPROJECT_NAME}_DARKSTYLE_RCC
    "rc/darkstyle.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/darkstyle.rcc"
)

qt5_add_binary_resources(${FWPROJECT_NAME}_FLATDARK_RCC
    "rc/flatdark.qrc"
    DESTINATION "${CMAKE_BINARY_DIR}/${FWBUNDLE_RC_PREFIX}/${${FWPROJECT_NAME}_FULLNAME}/flatdark.rcc"
)

add_dependencies(${FWPROJECT_NAME} ${FWPROJECT_NAME}_DARKSTYLE_RCC)
add_dependencies(${FWPROJECT_NAME} ${FWPROJECT_NAME}_FLATDARK_RCC)
