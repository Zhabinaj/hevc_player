cmake_minimum_required(VERSION 3.14)

project(hevc_player LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_PATH "${PROJECT_SOURCE_DIR}/../HEVC_PLAYER_BUILD")
set(Dir "${PROJECT_SOURCE_DIR}/..CMake_Trash/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}")
set(RUNTIME_OUTPUT_DIRECTORY "${BUILD_PATH}")
set(LIBRARY_OUTPUT_DIRECTORY "${BUILD_PATH}")
set(ARCHIVE_OUTPUT_DIRECTORY "${BUILD_PATH}")

set(USE_ASAN OFF)

if(USE_ASAN)
  set(CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address")
endif()

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(EXTERNAL_LIBS_PATH "${PROJECT_SOURCE_DIR}/../kpp_libs/linux")
else()
    set(EXTERNAL_LIBS_PATH "${PROJECT_SOURCE_DIR}/../../kpp_libs/win")
endif()

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR})
message ("QT_VERSION_MAJOR " ${QT_VERSION_MAJOR})
find_package(Qt5 COMPONENTS Core Gui Qml Quick Widgets REQUIRED)
find_package(Threads REQUIRED)

set(BOOST_PATH "${EXTERNAL_LIBS_PATH}/boost")
find_package(Boost COMPONENTS filesystem REQUIRED PATHS ${BOOST_PATH})
include_directories(${CMAKE_FIND_ROOT_PATH}/boost/include/boost-1_74)


add_compile_options(
    -Wall
    -Wno-unknown-pragmas #вырубает ругань на pragma
)

include(Version.cmake)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(APP_ICON_RESOURCE_PATH "${PROJECT_SOURCE_DIR}/Resource/ico/icon.png") # Путь к иконке для Linux
    set(APP_ICON_RESOURCE_PATH "${PROJECT_SOURCE_DIR}/Resource/ico/alt_icon.png") # Путь к иконке для Linux
else()
    set(APP_ICON_RESOURCE_PATH "${PROJECT_SOURCE_DIR}/Resource/ico/icon.ico") # Путь к иконке для Windows
    set(APP_ICON_RESOURCE_PATH "${PROJECT_SOURCE_DIR}/Resource/ico/alt_icon.ico") # Путь к иконке для Windows
endif()

set(APP_ICON_RESOURCE "${APP_ICON_RESOURCE_PATH}") # Выбор иконки в зависимости от платформы

if(APP_ICON_RESOURCE)
    message ("if app icon")
    if(WIN32)
        set(ICON_FLAGS "-F pe-i386 -F pe-x86-64") # Флаги для Windows
        set(ICON_FILE "${APP_ICON_RESOURCE}")
        set(ICON_RC_FILE "${PROJECT_SOURCE_DIR}/Resource/ico/icon.rc")
        set(ICON_FLAGS "${ICON_FLAGS}" PARENT_SCOPE)
        set(ICON_FILE "${ICON_FILE}" PARENT_SCOPE)
        set(ICON_RC_FILE "${ICON_RC_FILE}" PARENT_SCOPE)
    else()
        set(ICON_FILE "${APP_ICON_RESOURCE_LINUX}" PARENT_SCOPE)
    endif()
endif()
message("icone file " ${ICON_FILE})
message("path " ${APP_ICON_RESOURCE_PATH})


if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    add_executable(hevc_player
      main.cpp
      image_provider.cpp session.cpp image_provider.h sei_data.h session.h
      Resource/rc.qrc ${ICON_RC_FILE}

      video_output.h
      video_output.cpp
      hevc_qimage_engine.h hevc_qimage_engine.cpp
      player.h player.cpp
    )
else() #чтобы не открывалась консоль при запуске приложения
    add_executable(hevc_player WIN32
      main.cpp
      image_provider.cpp session.cpp image_provider.h sei_data.h session.h
      Resource/rc.qrc ${ICON_RC_FILE}

      video_output.h
      video_output.cpp
      hevc_qimage_engine.h hevc_qimage_engine.cpp
      player.h player.cpp
    )
endif()

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
   include_directories(${EXTERNAL_LIBS_PATH})
else()
    include_directories(${EXTERNAL_LIBS_PATH}/ffmpeg-n4.4/include/)
endif()

message(${Boost_LIBRARIES})
target_link_libraries(hevc_player
     Qt5::Core
     Qt5::Gui
     Qt5::Qml
     Qt5::Quick
     Qt5::Widgets
     Threads::Threads
    )

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME}
        ${EXTERNAL_LIBS_PATH}
        avformat
        avcodec
        swscale
        avutil
        ${Boost_LIBRARIES}
    )
else()
    target_link_libraries(${PROJECT_NAME}
         ${EXTERNAL_LIBS_PATH}/ffmpeg-n4.4/bin/avformat-58.dll
         ${EXTERNAL_LIBS_PATH}/ffmpeg-n4.4/bin/avcodec-58.dll
         ${EXTERNAL_LIBS_PATH}/ffmpeg-n4.4/bin/swscale-5.dll
         ${EXTERNAL_LIBS_PATH}/ffmpeg-n4.4/bin/avutil-56.dll        
         ${Boost_LIBRARIES}
     )
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    message(STATUS "host linux")
else()
    message(STATUS "host windows")
    target_compile_definitions(${PROJECT_NAME} PUBLIC WINDOWS)
endif()

target_include_directories( ${PROJECT_NAME} PUBLIC view)

target_compile_options(${PROJECT_NAME} PUBLIC  -Wall -Wextra -lstdc++)

target_sources( ${PROJECT_NAME} PUBLIC ${HEADERS} ${SOURCES} )

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${PROJECT_SOURCE_DIR}/Resource" "${BUILD_PATH}/Resource")


include(GNUInstallDirs)
install(TARGETS hevc_player
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
