package require vtk 5.9

foreach s {colors mccases backdrop grab} {
  source [file join [file dirname [info script]] "${s}.tcl"]
}


package provide vtktesting 5.9
