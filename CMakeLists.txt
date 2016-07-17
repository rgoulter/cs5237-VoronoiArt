# Voronoi Art CMakeLists
#
# Two Executables,
# - the main qt application
# - the GTests

cmake_minimum_required(VERSION 2.8)

option(test "Build all tests." OFF)

project(VoronoiArt)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

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
set(VORONOI_LIB_SOURCES src/delaunay/li.cpp
                        src/delaunay/lmath.cpp
                        src/delaunay/polygon.cpp
                        src/delaunay/triangle.cpp
                        src/delaunay/triangulation.cpp
                        src/delaunay/pointsetio.cpp
                        src/delaunay/pointsetarray.cpp
                        src/delaunay/dagnode.cpp
                        src/delaunay/directedgraph.cpp
                        src/delaunay/delaunay.cpp
                        src/voronoi/polyconstruct.cpp
                        src/voronoi/voronoi.cpp
                        src/voronoi/vparabola.cpp
                        src/imagedata.cpp
                        src/generatepoints.cpp
                        src/polypixel.cpp)
# For Qt code
set(VORONOI_SOURCES src/mypanelopengl.cpp
                    src/mainqt.cpp)

set(VORONOI_CLI_SOURCES src/main_cli.cpp)

set(VORONOI_LIB_HEADERS include/delaunay/li.h
                        include/delaunay/lmath.h
                        include/delaunay/polygon.h
                        include/delaunay/triangle.h
                        include/delaunay/trianglegeometry.h
                        include/delaunay/triangulation.h
                        include/delaunay/pointsetio.h
                        include/delaunay/pointsetarray.h
                        include/delaunay/dagnode.h
                        include/delaunay/directedgraph.h
                        include/delaunay/delaunay.h
                        include/voronoi/polyconstruct.h
                        include/voronoi/vedge.h
                        include/voronoi/vevent.h
                        include/voronoi/vparabola.h
                        include/voronoi/vpoint.h
                        include/voronoi/voronoi.h
                        include/geometry/point.h
                        include/imagedata.h
                        include/generatepoints.h
                        include/polypixel.h)
set (VORONOI_HEADERS include/mypanelopengl.h
                     include/mainqt.h)

include_directories(include)


IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (PLATFORM_SOURCES src/linux/stopwatch.cpp)
    set (PLATFORM_HEADERS include/linux/platform.h
                          include/linux/stopwatch.h)
    include_directories(include/linux include/linux/basics)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF (APPLE)
    set (PLATFORM_SOURCES src/linux/stopwatch.cpp)
    set (PLATFORM_HEADERS include/mac/platform.h
                          include/mac/stopwatch.h)
    include_directories(include/mac include/mac/basics)
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

add_executable(VoronoiCLI ${VORONOI_CLI_SOURCES}
                          ${PLATFORM_HEADERS}
                          ${PLATFORM_SOURCES})

target_link_libraries(VoronoiCLI VoronoiArt_lib)
target_link_libraries(VoronoiCLI ${OpenCV_LIBS})
target_link_libraries(VoronoiCLI ${OPENGL_LIBRARIES})

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
  add_executable(runUnitTests test/delaunay/polygontests.cpp
                              test/delaunay/pointsettests.cpp
                              test/delaunay/pointsetiotests.cpp
                              test/delaunay/dagtests.cpp
                              test/delaunay/triangletests.cpp
                              test/delaunay/triangulationtests.cpp
                              test/voronoi/polyconstructtests.cpp
                              test/testmain.cpp)

  target_link_libraries(runUnitTests gtest gtest_main)
  target_link_libraries(runUnitTests VoronoiArt_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME voronoi-art-unit-test COMMAND runUnitTests)
endif()

