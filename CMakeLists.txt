# Voronoi Art CMakeLists
#
# Two Executables,
# - the main qt application
# - the GTests

cmake_minimum_required(VERSION 2.8)

option(test "Build all tests." OFF)

project(VoronoiArt)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Find dependencies
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)



# Lib for Non-Qt code
set(VORONOI_LIB_SOURCES src/basics/pointSet.cpp
                        src/basics/lmath.cpp
                        src/basics/li.cpp
                        src/basics/polygon.cpp
                        src/basicsP2/trist.cpp
                        src/basicsP2/pointSetArray.cpp
                        src/basicsP3/directedGraph.cpp
                        src/delaunay.cpp
                        src/Voronoi.cpp
                        src/VParabola.cpp
                        src/imagedata.cpp
                        src/generateProbabilityDistribution.cpp
                        src/polypixel.cpp)
# For Qt code
set(VORONOI_SOURCES src/mypanelopengl.cpp
                    src/mainqt.cpp)

set(VORONOI_LIB_HEADERS include/basics/polygon.h
                        include/basics/li.h
                        include/basics/pointSet.h
                        include/basics/lmath.h
                        include/basicsP2/trist.h
                        include/basicsP2/pointSetArray.h
                        include/basicsP3/directedGraph.h
                        include/delaunay.h
                        include/delaunayTri.h
                        include/VEdge.h
                        include/VEvent.h
                        include/VParabola.h
                        include/VPoint.h
                        include/Voronoi.h
                        include/imagedata.h
                        include/generateProbabilityDistribution.h
                        include/polypixel.h)
set (VORONOI_HEADERS include/mypanelopengl.h
                     include/mainqt.h)

include_directories(include
                    include/basics
                    include/basicsP2
                    include/basicsP3)


IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (PLATFORM_SOURCES src/linux/basics/stopWatch.cpp)
    set (PLATFORM_HEADERS include/linux/platform.h
                          include/linux/basics/stopWatch.h)
    include_directories(include/linux include/linux/basics)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (APPLE)
    set (PLATFORM_SOURCES src/linux/basics/stopWatch.cpp)
    set (PLATFORM_HEADERS include/mac/platform.h
                          include/mac/basics/stopWatch.h)
    include_directories(include/mac include/mac/basics soil/src)
ENDIF (APPLE)


qt5_wrap_ui(UI_HEADERS ./forms/mainqt.ui)
# Do we need this? Form in ./forms/mainqt.ui .. but unused.
# qt5_add_resources(RESOURCE_FILES ../resources/resources.qrc)


# Set some Win32 Specific Settings
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set some Apple MacOS Specific settings
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)


add_library(VoronoiArt_lib ${VORONOI_LIB_HEADERS}
                           ${VORONOI_LIB_SOURCES})
add_executable(VoronoiArt ${GUI_TYPE}
                          ${VORONOI_HEADERS}
                          ${VORONOI_SOURCES}
                          ${PLATFORM_HEADERS}
                          ${PLATFORM_SOURCES}
                          ${UI_HEADERS})

target_link_libraries(VoronoiArt VoronoiArt_lib)
target_link_libraries(VoronoiArt ${OpenCV_LIBS})
target_link_libraries(VoronoiArt ${OPENGL_LIBRARIES})

IF (APPLE)
ENDIF (APPLE)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")


# Use the Widgets module from Qt 5.
# Core, Gui, Widgets, OpenGL.
qt5_use_modules(VoronoiArt Widgets OpenGL)



# GTest integration with CMake adapted from
#   https://github.com/dmonopoly/gtest-cmake-example

################################
# Testing
################################
if (test)
  # This adds another subdirectory, which has 'project(gtest)'.
  add_subdirectory(gtest-1.7.0)

  enable_testing()

  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ##############
  # Unit Tests
  ##############
  add_executable(runUnitTests test/basictests.cpp)

  target_link_libraries(runUnitTests gtest gtest_main)
  target_link_libraries(runUnitTests VoronoiArt_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME voronoi-art-unit-test COMMAND runUnitTests)
endif()

