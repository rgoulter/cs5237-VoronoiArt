# Voronoi Art CMakeLists
#
# Two Executables,
# - the main qt application
# - the GTests

cmake_minimum_required(VERSION 2.8)

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


set(VORONOI_SOURCES src/basicsP3/directedGraph.cpp
                    src/polypixel.cpp
                    src/generateProbabilityDistribution.cpp
                    src/mypanelopengl.cpp
                    src/Voronoi.cpp
                    src/imagedata.cpp
                    src/delaunay.cpp
                    src/basicsP2/trist.cpp
                    src/basicsP2/pointSetArray.cpp
                    src/mainqt.cpp
                    src/VParabola.cpp
                    src/basics/pointSet.cpp
                    src/basics/lmath.cpp
                    src/basics/li.cpp
                    src/basics/polygon.cpp)

set (VORONOI_HEADERS    src/VEdge.h
                        src/VEvent.h
                        src/VParabola.h
                        src/VPoint.h
                        src/Voronoi.h
                        include/imagedata.h
                        include/basicsP3/directedGraph.h
                        include/delaunayTri.h
                        include/delaunay.h
                        include/tinydir.h
                        include/mypanelopengl.h
                        include/generateProbabilityDistribution.h
                        include/polypixel.h
                        include/basicsP2/trist.h
                        include/basicsP2/pointSetArray.h
                        include/mainqt.h
                        include/basics/polygon.h
                        include/basics/li.h
                        include/basics/pointSet.h
                        include/basics/lmath.h)

include_directories(include include/basics include/basicsP2 include/basicsP3)


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


add_executable(VoronoiArt ${GUI_TYPE}
                          ${VORONOI_HEADERS}
                          ${VORONOI_SOURCES}
                          ${PLATFORM_HEADERS}
                          ${PLATFORM_SOURCES}
                          ${UI_HEADERS})

target_link_libraries(VoronoiArt ${OpenCV_LIBS})
target_link_libraries(VoronoiArt ${OPENGL_LIBRARIES})

IF (APPLE)
ENDIF (APPLE)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")


# Use the Widgets module from Qt 5.
# Core, Gui, Widgets, OpenGL.
qt5_use_modules(VoronoiArt Widgets OpenGL)
