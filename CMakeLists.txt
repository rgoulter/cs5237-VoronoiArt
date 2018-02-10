# Voronoi Art CMakeLists
#
# Two Executables,
# - the main qt application
# - the GTests

cmake_minimum_required(VERSION 3.1)

option(test "Build all tests." OFF)

project(VoronoiArt)

# MSVC doesn't like -Wextra
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

# We need C++11 stantard.
set(CMAKE_CXX_STANDARD 11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Find dependencies
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})


# Lib for Non-Qt code
set(VORONOI_LIB_SOURCES src/delaunay/longint/li.cpp
                        src/delaunay/longint/lmath.cpp
                        src/delaunay/triangle.cpp
                        src/delaunay/triangulation.cpp
                        src/delaunay/pointsetio.cpp
                        src/delaunay/pointsetarray.cpp
                        src/delaunay/dagnode.cpp
                        src/delaunay/directedgraph.cpp
                        src/delaunay/delaunay.cpp
                        src/geometry/linesegment.cpp
                        src/geometry/polygon.cpp
                        src/imagedata.cpp
                        src/generatepoints.cpp
                        src/polypixel.cpp)
# For Qt code
set(VORONOI_SOURCES src/mypanelopengl.cpp
                    src/mainqt.cpp)

set(VORONOI_CLI_SOURCES src/main_cli.cpp)

set(VORONOI_LIB_HEADERS include/delaunay/longint/li.h
                        include/delaunay/longint/lmath.h
                        include/delaunay/polygon.h
                        include/delaunay/triangle.h
                        include/delaunay/trianglegeometry.h
                        include/delaunay/triangulation.h
                        include/delaunay/pointsetio.h
                        include/delaunay/pointsetarray.h
                        include/delaunay/dagnode.h
                        include/delaunay/directedgraph.h
                        include/delaunay/delaunay.h
                        include/geometry/linesegment.h
                        include/geometry/point.h
                        include/geometry/polygon.h
                        include/imagedata.h
                        include/generatepoints.h
                        include/polypixel.h
                        include/tracing.h)
set(VORONOI_HEADERS include/mypanelopengl.h
                    include/mainqt.h)

include_directories(include)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_SOURCES src/linux/stopwatch.cpp)
    set(PLATFORM_HEADERS include/linux/platform.h
                         include/linux/stopwatch.h)
    include_directories(include/linux include/linux/basics)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(APPLE)
    set(PLATFORM_SOURCES src/linux/stopwatch.cpp)
    set(PLATFORM_HEADERS include/mac/platform.h
                         include/mac/stopwatch.h)
    include_directories(include/mac include/mac/basics)
endif(APPLE)

if(WIN32)
    set(PLATFORM_SOURCES src/win32/stopwatch.cpp)
    set(PLATFORM_HEADERS include/win32/platform.h
                         include/win32/stopwatch.h)
    include_directories(include/win32)
endif(WIN32)

qt5_wrap_ui(UI_HEADERS ./forms/mainqt.ui)


# Set some Win32 Specific Settings
if(WIN32)
set(GUI_TYPE WIN32)
endif(WIN32)
# Set some Apple MacOS Specific settings
if(APPLE)
set(GUI_TYPE MACOSX_BUNDLE)
endif(APPLE)


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

if(APPLE)
endif(APPLE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


# Use the Widgets module from Qt 5.
# Core, Gui, Widgets, OpenGL.
qt5_use_modules(VoronoiArt Widgets OpenGL)



################################
# Testing
################################
if(test)
  # Use CTest
  enable_testing()

  ##############
  # Unit Tests
  ##############
  add_executable(runUnitTests test/geometry/polygontests.cpp
                              test/geometry/linesegmenttests.cpp
                              test/delaunay/pointsettests.cpp
                              test/delaunay/pointsetiotests.cpp
                              test/delaunay/dagtests.cpp
                              test/delaunay/triangletests.cpp
                              test/delaunay/triangulationtests.cpp
                              test/testmain.cpp)

  target_link_libraries(runUnitTests VoronoiArt_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  add_test(NAME voronoi-art-unit-test COMMAND runUnitTests)
endif()

