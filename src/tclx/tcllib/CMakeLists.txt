project(tclsrc C)

set(TCL_SRCS array.tcl assign.tcl edprocs.tcl forfile.tcl
	forrec.tcl globrec.tcl help.tcl packages.tcl
	pushd.tcl setfuncs.tcl showproc.tcl strfile.tcl
	compat.tcl convlib.tcl profrep.tcl)
	
set(TCL.TLIB ../tcllib/tcl.tlb)
set(TCL.TNDX ../tcllib/tcl.tdx)

add_custom_command(OUTPUT ${TCL.TLIB}  
	DEPENDS ${TCL_SRCS}
	COMMAND cat $(TCL_SRCS) | egrep -v '^#$$|^#  *|^#-|^#=|^  *#' > ${TCL.TLIB}
)
