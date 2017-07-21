cmake_minimum_required(VERSION 2.8)

project(sge)
set(PROJECT_SOURCE_DIR "../src/")

if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
  endif()
else()
  set (CMAKE_CXX_STANDARD 11)
endif()

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")

set(CMAKE_BUILD_TYPE Release)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
configure_file(
  "${PROJECT_BINARY_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if(APPLE)
  target_link_libraries(${PROJECT_NAME} "-framework sfml-window -framework sfml-graphics -framework Cocoa -framework sfml-system -framework sfml-audio")
elseif(WIN32)
  target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
elseif(UNIX)
  # TODO: not enough to just assume non-mac will be linux, there's BSD, etc...
  find_package(Threads)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(${PROJECT_NAME} "-lsfml-window -lsfml-graphics -lsfml-system -lsfml-network -lsfml-audio")
endif()

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
