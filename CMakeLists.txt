cmake_minimum_required(VERSION 2.8)

PROJECT(VTKIT)

# Need to include class headers and the configuration header.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

IF(WIN32)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF(WIN32)

# Setup output paths.
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

# All libraries are built statically
SET (BUILD_SHARED_LIBS OFF)
ADD_DEFINITIONS(-DSTATIC_BUILD)

# Build the Metakit library, mk.lib
SUBDIRS(Mk4tcl)

# Find and load VTK settings.
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# Error if users trys to link shared VTK libs
IF(VTK_BUILD_SHARED_LIBS)
   MESSAGE(SEND_ERROR "VTKit requires static VTK libraries")
ENDIF(VTK_BUILD_SHARED_LIBS)

# Build the VTKit basekit
SUBDIRS(tclkit)

# Build the VTKit runtime

