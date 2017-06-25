cmake_minimum_required(VERSION 3.5)

project(Engine)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(PROJECT_SOURCE_DIR "../src/")
set(JSON_DIR "../deps/JSON")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")

set(CMAKE_BUILD_TYPE Release)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
configure_file(
  "${PROJECT_BINARY_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )

if(WIN32)
  link_directories(${SFML_DIR}/lib/Release)
endif()

add_executable(${EXECUTABLE_NAME} WIN32 ${SOURCES})

include_directories("${JSON_DIR}")
if(APPLE)
  include(FindPkgConfig)
  pkg_search_module(GUILE REQUIRED guile-2.2)
  include_directories(${GUILE_INCLUDE_DIRS})
  link_directories(${GUILE_LIBRARY_DIRS})
  add_definitions(${GUILE_CFLAGS_OTHER})
  target_link_libraries(${EXECUTABLE_NAME} "-framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-audio" ${GUILE_LIBRARIES})
  # set(MACOSX_BUNDLE_BUNDLE_NAME ${EXECUTABLE_NAME})
  # set(MACOSX_BUNDLE_ICON_FILE icon.icns)
  # add_custom_target(pkg ALL mkdir -p ${EXECUTABLE_NAME}.app/Contents/Resources
  #   COMMAND mkdir -p ${EXECUTABLE_NAME}.app/Contents/Frameworks
  #   COMMAND cp -r ../res/* ${EXECUTABLE_NAME}.app/Contents/Resources
  #   COMMAND cp macos/icon.icns ${EXECUTABLE_NAME}.app/Contents/Resources
  #   COMMAND cp -r ${SFML_DIR}/lib/* ${EXECUTABLE_NAME}.app/Contents/Frameworks
  #   COMMAND cp -r ${SFML_DIR}/extlibs/libs-osx/Frameworks/* ${EXECUTABLE_NAME}.app/Contents/Frameworks)
elseif(WIN32)
  # FIXME: how to get path to show up in visual studio's "additional library directories"?
  target_link_libraries(${EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
elseif(UNIX)
  # TODO: not enough to just assume non-mac will be linux, there's BSD, etc...
  find_package(Threads)
  target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()
