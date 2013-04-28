cmake_minimum_required(VERSION 2.8)

project(VTKIT)


# Find upx and sdx utilities
find_program(UPX_COMMAND upx NAMES upx upx.exe DOC "Path to UPX executable")
find_program(SDX_COMMAND sdx NAMES sdx sdx.exe DOC "Path to SDX executable")

# Optionally link tcl's dde and registry libs on Windows
if(WIN32)
FIND_PATH(TCL_WIN_EXTRA_LIBS NAMES tcldde13s.lib tclreg12s.lib
    PATHS "${VTK_TCL_INCLUDE_PATH}/../win/Release_VC10" "${VTK_TCL_INCLUDE_PATH}/../win/Release"
    DOC "Path to tcldde and tclreg libraries")
endif()

# Need to include class headers and the configuration header.
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_BINARY_DIR})

# Kill squawks on Windows
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

## Generate config.h - tq rivet
include (CheckFunctionExists)
include (CheckTypeSize)
include (CheckLibraryExists)
include (CheckIncludeFiles)
check_include_files   (sys/stat.h  HAVE_SYS_STAT_H)
check_include_files   (sys/types.h HAVE_SYS_TYPES_H)
check_include_files   (stddef.h    STDC_HEADERS)
check_include_files   (dlfcn.h     HAVE_DLFCN_H)
check_include_files   (inttypes.h  HAVE_INTTYPES_H)
check_include_files   (limits.h    HAVE_LIMITS_H)
check_function_exists (lseek64     HAVE_LSEEK64)
check_include_files   (memory.h    HAVE_MEMORY_H)
check_include_files   (net/errno.h HAVE_NET_ERRNO_H)
check_function_exists (open64      HAVE_OPEN64)
check_include_files   (stdint.h    HAVE_STDINT_H)
check_include_files   (stdlib.h    HAVE_STDLIB_H)
check_include_files   (strings.h   HAVE_STRINGS_H)
check_include_files   (string.h    HAVE_STRING_H)
set(CMAKE_EXTRA_INCLUDE_FILES sys/types.h)
check_type_size       ("struct dirent64" HAVE_STRUCT_DIRENT64)
set(CMAKE_EXTRA_INCLUDE_FILES)
set(CMAKE_EXTRA_INCLUDE_FILES sys/stat.h)
check_type_size       ("struct stat64" HAVE_STRUCT_STAT64)
set(CMAKE_EXTRA_INCLUDE_FILES)
check_type_size       ("long" SIZEOF_LONG)
check_include_files   (sys/param.h HAVE_SYS_PARAM_H)
set(CMAKE_EXTRA_INCLUDE_FILES sys/types.h)
check_type_size       (off64_t     HAVE_TYPE_OFF64_T)
set(CMAKE_EXTRA_INCLUDE_FILES)
check_include_files   (unistd.h    HAVE_UNISTD_H)
check_include_files   (pwd.h       HAVE_PWD_H)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
ADD_DEFINITIONS   ( -DHAVE_CONFIG_H=1 )

# For some reason sys/stat.h not found on mac osx, when it should be.
if(APPLE)
   add_definitions( -DHAVE_SYS_STAT_H)
endif()

# Build the Metakit library, mk.lib
SUBDIRS(Mk4tcl)

# Find and load VTK settings.
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

# Error if users trys to link shared VTK libs
if(VTK_BUILD_SHARED_LIBS)
   message(SEND_ERROR "VTKit requires static VTK libraries")
endif()

# Build the VTKit runtime

# Build the VTKit basekit
subdirs(tclkit)
