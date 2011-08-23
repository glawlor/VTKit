foreach kit {Common Filtering IO Imaging Graphics Rendering Hybrid VolumeRendering Widgets} {
    load {} vtk${kit}Tcl
}


if {[info exists ::env(VTK_DATA_ROOT)]} {
    set ::VTK_DATA_ROOT $::env(VTK_DATA_ROOT)
} else {
#     This might be handy or it could be a pain...
#
#     set msg "The VTK_DATA_ROOT environment variable is not set!\n"
#     append msg "Do you want to temporarily set a directory as its value now?"
#     set ans [tk_messageBox -icon warning -default ok -type okcancel -message $msg -title "Warning!"]
#     switch $ans {
#         ok {
#             set d [tk_chooseDirectory -title "Find the VTKData directory" -mustexist 1]
#             if {$d != ""} {
#                 set ::VTK_DATA_ROOT $d
#             } 
#         }
#         cancel {
#             #maybe have a guess here.
#         }
#     }
}

if {[info commands ::vtk::exit] eq "" } {
  rename ::exit ::vtk::exit
  proc ::exit {{returnCode 0}} {
    vtkCommand DeleteAllObjects
    return [::vtk::exit $returnCode]
  }
}

package provide vtk 5.9

