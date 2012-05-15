project(Mk4tcl)

include_directories(BEFORE ${Mk4tcl_SOURCE_DIR})
include_directories("${VTK_TCL_INCLUDE_DIR}")

#Source files for metakit
set(Mk4tcl_SRCS
remap.cpp
fileio.cpp
view.cpp
handler.cpp
field.cpp
table.cpp
format.cpp
persist.cpp
store.cpp
viewx.cpp
string.cpp
custom.cpp
univ.cpp
derived.cpp
std.cpp
column.cpp
mk4tcl.cpp
mk4too.cpp
)

add_library(Mk4tcl ${Mk4tcl_SRCS})
