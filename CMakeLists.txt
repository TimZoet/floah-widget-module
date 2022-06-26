set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../floah-layout")
include(floahVersionString)

set(NAME floah-widget)
set(TYPE module)
set(INCLUDE_DIR "include/floah-widget")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/layer.h
    ${INCLUDE_DIR}/panel.h

    ${INCLUDE_DIR}/generators/scenegraph_generator.h

    ${INCLUDE_DIR}/widgets/button.h
    ${INCLUDE_DIR}/widgets/checkbox.h
    ${INCLUDE_DIR}/widgets/dropdown.h
    ${INCLUDE_DIR}/widgets/radio_button.h
    ${INCLUDE_DIR}/widgets/widget.h
)

set(SOURCES
    ${SRC_DIR}/layer.cpp
    ${SRC_DIR}/panel.cpp

    ${SRC_DIR}/generators/scenegraph_generator.cpp

    ${SRC_DIR}/widgets/button.cpp
    ${SRC_DIR}/widgets/checkbox.cpp
    ${SRC_DIR}/widgets/dropdown.cpp
    ${SRC_DIR}/widgets/radio_button.cpp
    ${SRC_DIR}/widgets/widget.cpp
)

set(DEPS_PUBLIC
    floah-common
    floah-data
    floah-layout
    floah-put
    floah-viz
)

set(DEPS_PRIVATE

)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${FLOAH_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${FLOAH_VERSION}
    HEADERS "${HEADERS}"
)

target_compile_definitions(
    ${NAME}
    PRIVATE
        FLOAH_VERSION_MAJOR=${FLOAH_VERSION_MAJOR}
        FLOAH_VERSION_MINOR=${FLOAH_VERSION_MINOR}
        FLOAH_VERSION_PATCH=${FLOAH_VERSION_PATCH}
)
