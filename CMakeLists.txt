cmake_minimum_required(VERSION 2.8)

project(VTKIT)

find_program(UPX_COMMAND upx NAMES upx upx.exe DOC "Path to UPX executable")
find_program(SDX_COMMAND sdx NAMES sdx sdx.exe DOC "Path to SDX executable")
FIND_PATH(TCL_WIN_EXTRA_LIBS NAMES tcldde13s.lib tclreg12s.lib
    PATHS "${VTK_TCL_INCLUDE_PATH}/../win/Release_VC10" "${VTK_TCL_INCLUDE_PATH}/../win/Release"
    DOC "Path to tcldde and tclreg libraries")

# Need to include class headers and the configuration header.
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

if(WIN32)
   add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

# Setup output paths.
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH
    "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)

# All libraries are built statically
set(BUILD_SHARED_LIBS OFF)
add_definitions(-DSTATIC_BUILD)

# Build the Metakit library, mk.lib
SUBDIRS(Mk4tcl)

# Find and load VTK settings.
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# Error if users trys to link shared VTK libs
if(VTK_BUILD_SHARED_LIBS)
   message(SEND_ERROR "VTKit requires static VTK libraries")
endif()

# Build the VTKit basekit
subdirs(tclkit)

# Build the VTKit runtime
