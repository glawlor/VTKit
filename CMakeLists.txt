cmake_minimum_required(VERSION 2.8)

PROJECT(VTKIT)

FIND_PROGRAM(UPX_COMMAND upx NAMES upx upx.exe DOC "Path to UPX executable")
FIND_PROGRAM(SDX_COMMAND sdx NAMES sdx sdx.exe DOC "Path to SDX executable")
FIND_PATH(TCL_WIN_EXTRA_LIBS NAMES tcldde13s.lib tclreg12s.lib
    PATHS "${VTK_TCL_INCLUDE_PATH}/../win/Release_VC9" "${VTK_TCL_INCLUDE_PATH}/../win/Release"
    DOC "Path to tcldde and tclreg libraries")

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
