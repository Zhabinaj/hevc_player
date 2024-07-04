

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
include (GitVersioner.cmake)

project(${PROJECT_NAME} VERSION  ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
if (NOT "${VERSION_DIRTY}" STREQUAL "")
    set(PROJECT_VERSION "${PROJECT_VERSION}.${VERSION_SHA}.${VERSION_COMMIT}.${date}-${VERSION_DIRTY}")
else()
    set(PROJECT_VERSION "${PROJECT_VERSION}.${VERSION_SHA}.${VERSION_COMMIT}.${date}")
endif()

set(PROJECT_VERSION "${PROJECT_VERSION}")

message(STATUS "================================================")
message(STATUS "PROJECT_VERSION_MAJOR: ${PROJECT_VERSION_MAJOR}")
message(STATUS "PROJECT_VERSION_MINOR: ${PROJECT_VERSION_MINOR}")
message(STATUS "PROJECT_SHA: ${VERSION_SHA}")
message(STATUS "NUMBER_COMMIT: ${VERSION_COMMIT}")
message(STATUS "VERSION_PATCH: ${VERSION_PATCH}")
message(STATUS "VERSION_DIRTY: ${VERSION_DIRTY}")
message(STATUS "FULL-Version: ${PROJECT_VERSION}")
message(STATUS "================================================")


message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")


configure_file (
    "${PROJECT_SOURCE_DIR}/Version/Version.h.in"
    "${PROJECT_SOURCE_DIR}/Version/Version.h"
)

