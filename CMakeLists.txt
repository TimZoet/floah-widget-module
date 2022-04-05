set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../floah")
include(floahVersionString)

set(NAME floah-widget)
set(TYPE module)
set(INCLUDE_DIR "include/floah-widget")
set(SRC_DIR "src")

set(HEADERS
    
)

set(SOURCES
    
)

set(DEPS_PUBLIC

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
