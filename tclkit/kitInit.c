/*
 * tclAppInit.c --
 *
 *  Provides a default version of the main program and Tcl_AppInit
 *  procedure for Tcl applications (without Tk).  Note that this
 *  program must be built in Win32 console mode to work properly.
 *
 * Copyright (c) 1996-1997 by Sun Microsystems, Inc.
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 * Copyright (c) 2000-2002 Jean-Claude Wippler <jcw@equi4.com>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: kitInit.c,v 1.30 2006/03/24 14:18:07 jcw Exp $
 */
#include "config.h"
#ifdef KIT_INCLUDES_TK
#include <tk.h>
#else
#include <tcl.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#ifndef MB_TASKMODAL
#define MB_TASKMODAL 0
#endif

#include "tclInt.h"

#ifdef KIT_INCLUDES_VTK
Tcl_AppInitProc Vtkcommontcl_Init, Vtkfilteringtcl_Init, Vtkgraphicstcl_Init;
Tcl_AppInitProc Vtkiotcl_Init, Vtkimagingtcl_Init;

#ifdef KIT_INCLUDES_VTK_RENDERING
  Tcl_AppInitProc Vtkwidgetstcl_Init, Vtkvolumerenderingtcl_Init;
  Tcl_AppInitProc Vtkrenderingtcl_Init, Vtkhybridtcl_Init;
#endif // KIT_INCLUDES_VTK_RENDERING

#ifdef KIT_INCLUDES_VTK_PARALLEL
  Tcl_AppInitProc Vtkparalleltcl_Init;
#endif // KIT_INCLUDES_VTK_PARALLEL

#ifdef KIT_INCLUDES_VTK_INFOVIS
  Tcl_AppInitProc Vtkinfovistcl_Init;
#endif // KIT_INCLUDES_VTK_INFOVIS

#ifdef KIT_INCLUDES_VTK_VIEWS
  Tcl_AppInitProc Vtkviewstcl_Init;
#endif // KIT_INCLUDES_VTK_VIEWS

#ifdef KIT_INCLUDES_BIOENG
Tcl_AppInitProc Vtkbioengtcl_Init;
#endif // KIT_INCLUDES_BIOENG

#endif // KIT_INCLUDES_VTK

#ifdef KIT_INCLUDES_TBCLOAD
Tcl_AppInitProc Tbcload_Init;
#endif // KIT_INCLUDES_TBCLOAD

#ifdef KIT_INCLUDES_ITCL
Tcl_AppInitProc	Itcl_Init;
#endif
#ifdef KIT_LITE
Tcl_AppInitProc	Thrive_Init;
#else
Tcl_AppInitProc	Mk4tcl_Init;
#endif
Tcl_AppInitProc	Vfs_Init, Rechan_Init, Zlib_Init;
#if 10 * TCL_MAJOR_VERSION + TCL_MINOR_VERSION < 85
Tcl_AppInitProc	Pwb_Init;
#endif
#ifdef TCL_THREADS
Tcl_AppInitProc	Thread_Init;
#endif
#ifdef KIT_INCLUDES_TCL_WIN_LIBS
  #ifdef _WIN32
  Tcl_AppInitProc	Dde_Init, Registry_Init;
  #endif
#endif

char *tclExecutableName;

#ifdef KIT_LITE

/* Tclkit Lite needs some "rom code" *before* it can even decode the MK data
 * at the end of the executable.  That script is located between the exe and
 * the MK data.  It is (optionally) compressed, hence the need for Zlib here.
 */

#include <zlib.h>

/* Read a 4-byte big-endian int from current file pos */

static int read4b(FILE* fd) {
  int i = (fgetc(fd) & 0xFF) << 24;
  i |= (fgetc(fd) & 0xFF) << 16;
  i |= (fgetc(fd) & 0xFF) << 8;
  return i | (fgetc(fd) & 0xFF);
}

/* Read the pre-init code, which is sandwiched between the exe and the MK data.
 * To do so, read from the end skipping the MK data, then get the tXiS marker,
 * which is 12 bytes: "tXis" chars, uncompressed length, compressed length.
 */

static char* getPreInitFromExe() {
  char* up = 0;
  if (tclExecutableName != 0 && *tclExecutableName != 0) {
    FILE *fd = fopen(tclExecutableName,"rb");
    if (fd != 0) {
      int mark = 0x74586953; /* tXiS */
      if (fseek(fd,-12,2) == 0 &&
	  fseek(fd,-read4b(fd)-20,1) == 0 &&
	  read4b(fd) == mark) {
	int u = read4b(fd);
	int z = read4b(fd);
	if (fseek(fd,-z-12,1) == 0) {
	  int ok = 0;
	  up = ckalloc(u+1);
	  up[u] = 0;
	  if (u == z)
	    ok = (int) fread(up,1,u,fd) == u;
	  else {
	    char* zp = ckalloc(z+2);
	    ok = (int) fread(zp,1,z,fd) == z;
	    if (ok) {
	      uLongf len = u;
	      ok = uncompress((Bytef*) up, &len, (const Bytef*) zp, z) == Z_OK
		    && (int) len == u;
	    }
	    ckfree(zp);
	  }
	  if (!ok) { ckfree(up); up = 0; }
	}
      }
      fclose(fd);
    }
  }
  return up;
}

#endif

    /*
     *  Attempt to load a "boot.tcl" entry from the embedded MetaKit file.
     *  If there isn't one, try to open a regular "setup.tcl" file instead.
     *  If that fails, this code will throw an error, using a message box.
     */

static char *preInitCmd =
#ifdef KIT_LITE
  0
#else
#ifdef _WIN32_WCE
/* silly hack to get wince port to launch, some sort of std{in,out,err} problem */
"open /kitout.txt a; open /kitout.txt a; open /kitout.txt a\n"
/* this too seems to be needed on wince - it appears to be related to the above */
"catch {rename source ::tcl::source}\n"
"proc source file {\n"
    "set old [info script]\n"
    "info script $file\n"
    "set fid [open $file]\n"
    "set data [read $fid]\n"
    "close $fid\n"
    "set code [catch {uplevel 1 $data} res]\n"
    "info script $old\n"
    "if {$code == 2} { set code 0 }\n"
    "return -code $code $res\n"
"}\n"
#endif
"proc tclKitInit {} {\n"
    "rename tclKitInit {}\n"
    "load {} Mk4tcl\n"
    "mk::file open exe [info nameofexecutable] -readonly\n"
    "set n [mk::select exe.dirs!0.files name boot.tcl]\n"
    "if {$n != \"\"} {\n"
        "set s [mk::get exe.dirs!0.files!$n contents]\n"
	"if {![string length $s]} { error \"empty boot.tcl\" }\n"
        "catch {load {} zlib}\n"
        "if {[mk::get exe.dirs!0.files!$n size] != [string length $s]} {\n"
	    "set s [zlib decompress $s]\n"
	"}\n"
    "} else {\n"
        "set f [open setup.tcl]\n"
        "set s [read $f]\n"
        "close $f\n"
    "}\n"
    "uplevel #0 $s\n"

#ifdef KIT_INCLUDES_TBCLOAD
    "package ifneeded tbcload 1.7 {load {} tbcload}\n"
#endif KIT_INCLUDES_TBCLOAD

"}\n"
"tclKitInit"
#endif /* KIT_LITE */
;

static const char initScript[] =
"if {[file isfile [file join [info nameofexe] main.tcl]]} {\n"
    "if {[info commands console] != {}} { console hide }\n"
    "set tcl_interactive 0\n"
    "incr argc\n"
    "set argv [linsert $argv 0 $argv0]\n"
    "set argv0 [file join [info nameofexe] main.tcl]\n"
"} else continue\n"
;

/* SetExecName --

   Hack to get around Tcl bug 1224888.
*/

void SetExecName(Tcl_Interp *interp) {
    if (tclExecutableName == NULL) {
	int len = 0;
	Tcl_Obj *execNameObj;
	Tcl_Obj *lobjv[1];

	lobjv[0] = Tcl_GetVar2Ex(interp, "argv0", NULL, TCL_GLOBAL_ONLY);
	execNameObj = Tcl_FSJoinToPath(Tcl_FSGetCwd(interp), 1, lobjv);

	tclExecutableName = strdup(Tcl_GetStringFromObj(execNameObj, &len));
    }
}

int
TclKit_AppInit(Tcl_Interp *interp)
{
#ifdef KIT_INCLUDES_VTK
    Tcl_StaticPackage(0, "vtkCommonTcl", Vtkcommontcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkFilteringTcl", Vtkfilteringtcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkGraphicsTcl", Vtkgraphicstcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkIOTcl", Vtkiotcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkImagingTcl", Vtkimagingtcl_Init, NULL);

#ifdef KIT_INCLUDES_VTK_RENDERING
    Tcl_StaticPackage(0, "vtkRenderingTcl", Vtkrenderingtcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkHybridTcl", Vtkhybridtcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkWidgetsTcl", Vtkwidgetstcl_Init, NULL);
    Tcl_StaticPackage(0, "vtkVolumeRenderingTcl", Vtkvolumerenderingtcl_Init, NULL);
#endif // KIT_INCLUDES_VTK_RENDERING

#ifdef KIT_INCLUDES_VTK_PARALLEL
  Tcl_StaticPackage(0, "vtkParallelTcl", Vtkparalleltcl_Init, NULL);
#endif // KIT_INCLUDES_VTK_PARALLEL

#ifdef KIT_INCLUDES_VTK_INFOVIS
  Tcl_StaticPackage(0, "vtkInfovisTcl", Vtkinfovistcl_Init, NULL);
#endif // KIT_INCLUDES_VTK_INFOVIS

#ifdef KIT_INCLUDES_VTK_VIEWS
  Tcl_StaticPackage(0, "vtkViewsTcl", Vtkviewstcl_Init, NULL);
#endif // KIT_INCLUDES_VTK_VIEWS

#ifdef KIT_INCLUDES_BIOENG
  Tcl_StaticPackage(0, "vtkBioengTcl", Vtkbioengtcl_Init, NULL);
#endif // KIT_INCLUDES_BIOENG

#endif // KIT_INCLUDES_VTK

#ifdef KIT_INCLUDES_TBCLOAD
    Tcl_StaticPackage(0, "tbcload", Tbcload_Init, NULL);
#endif // KIT_INCLUDES_TBCLOAD

#ifdef KIT_INCLUDES_ITCL
    Tcl_StaticPackage(0, "Itcl", Itcl_Init, NULL);
#endif

#ifdef KIT_LITE
    Tcl_StaticPackage(0, "thrive", Thrive_Init, NULL);
#else
    Tcl_StaticPackage(0, "Mk4tcl", Mk4tcl_Init, NULL);
#endif

#if 10 * TCL_MAJOR_VERSION + TCL_MINOR_VERSION < 85
    Tcl_StaticPackage(0, "pwb", Pwb_Init, NULL);
#endif

    Tcl_StaticPackage(0, "rechan", Rechan_Init, NULL);
    Tcl_StaticPackage(0, "vfs", Vfs_Init, NULL);
    Tcl_StaticPackage(0, "zlib", Zlib_Init, NULL);

#ifdef TCL_THREADS
    Tcl_StaticPackage(0, "Thread", Thread_Init, NULL);
#endif

#if KIT_INCLUDES_TCL_WIN_LIBS
#ifdef _WIN32
    Tcl_StaticPackage(0, "dde", Dde_Init, NULL);
    Tcl_StaticPackage(0, "registry", Registry_Init, NULL);
#endif
#endif

#ifdef KIT_INCLUDES_TK
    Tcl_StaticPackage(0, "Tk", Tk_Init, Tk_SafeInit);
#endif

    /* the tcl_rcFileName variable only exists in the initial interpreter */
#ifdef _WIN32
    Tcl_SetVar(interp, "tcl_rcFileName", "~/tclkitrc.tcl", TCL_GLOBAL_ONLY);
#else
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclkitrc", TCL_GLOBAL_ONLY);
#endif

    /* Hack to get around Tcl bug 1224888.  This must be run here and
     * in LibraryPathObjCmd because this information is needed both
     * before and after that command is run. */
    SetExecName(interp);

#ifdef KIT_LITE
    preInitCmd = getPreInitFromExe();
    if (preInitCmd == 0) {
	Tcl_SetResult(interp, "cannot find kit pre-init script", TCL_STATIC);
	goto error;
    }
#endif

    TclSetPreInitScript(preInitCmd);
    if (Tcl_Init(interp) == TCL_ERROR)
        goto error;

#ifdef KIT_INCLUDES_TK
    if (Tk_Init(interp) == TCL_ERROR)
        goto error;
#ifdef _WIN32
    if (Tk_CreateConsoleWindow(interp) == TCL_ERROR)
        goto error;
#endif
#endif

    /* messy because TclSetStartupScriptPath is called slightly too late */
    if (Tcl_EvalEx(interp, initScript, -1, TCL_EVAL_GLOBAL) == TCL_OK) {
	const char *encoding = NULL;
        Tcl_Obj* path = Tcl_GetStartupScript(&encoding);
      	Tcl_SetStartupScript(Tcl_GetObjResult(interp), encoding);
      	if (path == NULL) {
	    Tcl_Eval(interp, "incr argc -1; set argv [lrange $argv 1 end]");
	}
    }

    Tcl_SetVar(interp, "errorInfo", "", TCL_GLOBAL_ONLY);
    Tcl_ResetResult(interp);
    return TCL_OK;

error:
#ifdef KIT_INCLUDES_TK
#ifdef _WIN32
    MessageBeep(MB_ICONEXCLAMATION);
#ifndef _WIN32_WCE
    MessageBox(NULL, Tcl_GetStringResult(interp), "Error in TclKit",
        MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
    ExitProcess(1);
#endif
    /* we won't reach this, but we need the return */
#endif
#endif
    return TCL_ERROR;
}
