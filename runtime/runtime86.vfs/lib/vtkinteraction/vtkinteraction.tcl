package require vtk 5.10

foreach s {Interactor bindings-rw bindings-iw bindings setget} {
  source [file join [file dirname [info script]] "${s}.tcl"]
}

package provide vtkinteraction 5.10
