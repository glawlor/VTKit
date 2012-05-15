project(TCLKIT)

set(TCLKIT_SRCS
  kitInit.c
  rechan.c
  pwb.c
  vfs.c
  zlib.c
)

if(WIN32)
   set(VTKIT_EXE_RESOURCE_FILES tclkit.rc )
   add_definitions(-D_USE_32BIT_TIME_T)
   add_definitions(-DTK_LOCAL_APPINIT=\"TclKit_AppInit\")
   set(TCLKIT_SRCS ${TCLKIT_SRCS} winMain.c)
endif()

if(UNIX)
   set(TCLKIT_SRCS ${TCLKIT_SRCS} tclAppInit.c main.cxx)
   add_definitions(-DTCL_LOCAL_APPINIT=\"TclKit_AppInit\")
endif()

include_directories(${VTK_SOURCE_DIR}/Utilities/vtkzlib "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${VTK_TCL_INCLUDE_DIR}")
include_directories("${VTK_TK_INCLUDE_DIR}")
include_directories("${VTK_TK_INTERNAL_DIR}")
include_directories("${VTK_TK_INCLUDE_DIR}/../xlib")

if (WIN32)
  if (NOT BORLAND)
    if(NOT CYGWIN)
      include_directories("${VTK_TCL_INCLUDE_DIR}/../win")
      if(VTK_USE_TK)
        include_directories("${VTK_TK_RESOURCES_DIR}" "${VTK_TK_INTERNAL_DIR}")
      endif()
    endif()
  endif()
endif()

# Libraries are fully qualified so don't need these... I think
#link_directories(${VTK_TK_LIBRARY} ${VTK_TCL_LIBRARY})
link_directories(${TCL_WIN_EXTRA_LIBS})

add_definitions(-DKIT_INCLUDES_TK -DKIT_INCLUDES_VTK)
add_executable (tclkit WIN32 ${TCLKIT_SRCS} ${VTKIT_EXE_RESOURCE_FILES})

# Tcl/Tk is linked with /GL so tclkit could do with /LTCG ... on windows
#set_target_properties(tclkit PROPERTIES LINK_FLAGS  "${LINK_FLAGS} -LTCG")


target_link_libraries (tclkit
  Mk4tcl
  vtkCommonTCL
  vtkFilteringTCL
  vtkGraphicsTCL
  vtkImagingTCL
  vtkIOTCL
)

if (WIN32)
  TARGET_LINK_LIBRARIES(tclkit tcldde13s)
  TARGET_LINK_LIBRARIES(tclkit tclreg12s)
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
  ADD_DEFINITIONS(-DKIT_INCLUDES_VTK_PARALLEL)
endif ()

if(VTK_USE_INFOVIS)
  target_link_libraries(tclkit vtkInfovisTCL)
  ADD_DEFINITIONS(-DKIT_INCLUDES_VTK_INFOVIS)
endif ()

if(VTK_USE_VIEWS)
  TARGET_LINK_LIBRARIES(tclkit vtkViewsTCL)
  ADD_DEFINITIONS(-DKIT_INCLUDES_VTK_VIEWS)
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
endif()



if(UNIX)
  add_custom_command(TARGET tclkit
        POST_BUILD
        COMMAND ${UPX_COMMAND} -9 -f -o ../bin/VTKit-upx ../bin/tclkit
        ARGS
        COMMAND cat
        ARGS ${VTKIT_SOURCE_DIR}/runtime/runtime${TCLTK_VERSION}.kit >>../bin/VTKit-upx
        )
endif()
