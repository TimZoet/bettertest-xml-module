find_package(pugixml REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../bettertest")
include(bettertestVersionString)

set(NAME bettertest_xml)
set(TYPE module)
set(INCLUDE_DIR "include/bettertest_xml")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/xml_exporter.h
    ${INCLUDE_DIR}/xml_importer.h
)
 
set(SOURCES
    ${SRC_DIR}/xml_exporter.cpp
    ${SRC_DIR}/xml_importer.cpp
)

set(DEPS_PUBLIC
    bettertest
)

set(DEPS_PRIVATE
    pugixml::pugixml
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${BETTERTEST_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${BETTERTEST_VERSION}
    HEADERS "${HEADERS}"
)
