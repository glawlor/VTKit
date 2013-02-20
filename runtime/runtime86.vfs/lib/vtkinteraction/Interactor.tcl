# a generic interactor for tcl and vtk
#
# VTKit uses the tk console... dont need vtkInteract any more.
# Just in case someone tries to deiconify it, the hack below is provided
# You should use "iren AddObserver UserEvent {console show}" instead.

proc vtkInteract {} {
  toplevel .vtkInteract
  wm iconify .vtkInteract
  wm withdraw .vtkInteract
  bind .vtkInteract <Map> {wm withdraw .vtkInteract; console show}
}

vtkInteract
