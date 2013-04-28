project(TCLKIT)

set(TCLKIT_SRCS
  kitInit.c
  rechan.c
  pwb.c
  vfs.c
  zlib.c
)

set(VTKIT_TCL_SRC ${VTK_TCL_INCLUDE_DIR} CACHE PATH
    "The root of the Tcl source code tree")
set(VTKIT_TK_SRC ${VTK_TK_INCLUDE_DIR} CACHE PATH
    "The root of the Tk source code tree")

# Platform independent rules
include_directories("${VTKIT_TCL_SRC}/generic")
include_directories("${VTKIT_TK_SRC}/generic")
include_directories("${VTKIT_TK_SRC}/xlib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_BINARY_DIR}")
#include_directories(${VTK_SOURCE_DIR}/Utilities/vtkzlib "${CMAKE_CURRENT_SOURCE_DIR}")

#link_directories(${VTK_TK_LIBRARY} ${VTK_TCL_LIBRARY})
add_definitions(-DTK_LOCAL_APPINIT=TclKit_AppInit)
add_definitions(-DKIT_INCLUDES_TK -DKIT_INCLUDES_VTK)

# Platform dependent rules
if(WIN32)
   set(TCLKIT_SRCS ${TCLKIT_SRCS} ${VTKIT_TK_SRC}/win/winMain.c)
   set(VTKIT_EXE_RESOURCE_FILES tclkit.rc )
   add_definitions(-D_USE_32BIT_TIME_T)
   include_directories("${VTKIT_TCL_SRC}/win")
   include_directories("${VTKIT_TK_SRC}/win/rc")
   link_directories(${TCL_WIN_EXTRA_LIBS})
   add_executable(tclkit WIN32 ${TCLKIT_SRCS} ${VTKIT_EXE_RESOURCE_FILES})
   # Tcl/Tk is linked with /GL so tclkit could do with /LTCG ... on windows
   set_target_properties(tclkit PROPERTIES LINK_FLAGS  "${LINK_FLAGS} -LTCG")
else()
   set(TCLKIT_SRCS ${TCLKIT_SRCS} ${VTKIT_TK_SRC}/unix/tkAppInit.c)
   include_directories("${VTKIT_TCL_SRC}/unix")
   include_directories("${VTKIT_TK_SRC}/unix")
   if(APPLE)
      include_directories("${VTKIT_TK_SRC}/macosx}")
   endif()
   add_executable(tclkit ${TCLKIT_SRCS})
endif()

if(VTK_USE_COCOA)
    #Strange, but you have to link against the Carbon framework
    target_link_libraries(tclkit "-framework Carbon")
endif(VTK_USE_COCOA)

# Libraries to link against
target_link_libraries (tclkit
  Mk4tcl
  vtkCommonTCL
  vtkFilteringTCL
  vtkGraphicsTCL
  vtkImagingTCL
  vtkIOTCL
)

if (WIN32)
  target_link_libraries(tclkit tcldde13s)
  target_link_libraries(tclkit tclreg12s)
endif ()

if (VTK_USE_RENDERING)
  target_link_libraries(tclkit vtkRenderingTCL)
  target_link_libraries(tclkit vtkVolumeRenderingTCL)
  target_link_libraries(tclkit vtkHybridTCL)
  target_link_libraries(tclkit vtkWidgetsTCL)
  add_definitions(-DKIT_INCLUDES_VTK_RENDERING)
endif ()

if (VTK_USE_PARALLEL)
  target_link_libraries(tclkit vtkParallelTCL)
  add_definitions(-DKIT_INCLUDES_VTK_PARALLEL)
endif ()

if(VTK_USE_INFOVIS)
  target_link_libraries(tclkit vtkInfovisTCL)
  add_definitions(-DKIT_INCLUDES_VTK_INFOVIS)
endif ()

if(VTK_USE_VIEWS)
  target_link_libraries(tclkit vtkViewsTCL)
  add_definitions(-DKIT_INCLUDES_VTK_VIEWS)
endif()

include(${VTK_CMAKE_DIR}/vtkTclTkMacros.cmake)
VTK_GET_TCL_TK_VERSION ("TCLTK_MAJOR_VERSION" "TCLTK_MINOR_VERSION")
set (TCLTK_VERSION "${TCLTK_MAJOR_VERSION}${TCLTK_MINOR_VERSION}")

# Compress the binary and append the runtime.
if (WIN32)
  set (EXECUTABLE_OUTPUT_DIR "${VTKIT_BINARY_DIR}\\bin\\${CMAKE_CFG_INTDIR}")
  string(REGEX REPLACE "/" "\\\\" EOD ${EXECUTABLE_OUTPUT_DIR})
  string(REGEX REPLACE "/" "\\\\" VSD ${VTKIT_SOURCE_DIR})
  add_custom_command(TARGET tclkit
       POST_BUILD
       COMMAND ${UPX_COMMAND}
       ARGS -9 -f -o ${EOD}\\VTKit-upx.exe ${EOD}\\tclkit.exe
       COMMAND COPY
       ARGS /B ${EOD}\\VTKit-upx.exe+${VSD}\\Runtime\\runtime${TCLTK_VERSION}.kit ${EOD}\\VTKit.exe
       )
else()
  add_custom_command(TARGET tclkit
        POST_BUILD
        COMMAND ${UPX_COMMAND} -9 -f -o ../bin/VTKit-upx ../bin/tclkit
        ARGS
        COMMAND cat
        ARGS ../bin/VTKit-upx ${VTKIT_SOURCE_DIR}/runtime/runtime${TCLTK_VERSION}.kit > ../bin/VTKit
        )
endif()
