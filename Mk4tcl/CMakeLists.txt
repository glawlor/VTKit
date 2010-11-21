PROJECT(Mk4tcl)

INCLUDE_DIRECTORIES(BEFORE ${Mk4tcl_SOURCE_DIR})

#Source files for metakit
SET(Mk4tcl_SRCS 
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

ADD_LIBRARY(Mk4tcl ${Mk4tcl_SRCS})
