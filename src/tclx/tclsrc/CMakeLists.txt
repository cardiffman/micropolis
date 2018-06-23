cmake_minimum_required(VERSION 3.6)
project(tclsrc C)

set(TCL_SRCS array.tcl assign.tcl edprocs.tcl forfile.tcl
	forrec.tcl globrec.tcl help.tcl packages.tcl
	pushd.tcl setfuncs.tcl showproc.tcl strfile.tcl
	compat.tcl convlib.tcl profrep.tcl)
	
set(TCL.TLIB ../tcllib/tcl.tlb)
set(TCL.TNDX ../tcllib/tcl.tdx)

file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/artcl.sh CONTENT "cd ${CMAKE_SOURCE_DIR}; out=$1; shift; cat $* | egrep -v '^#$$|^#  *|^#-|^#=|^  *#' > $out")
#execute_process(COMMAND chmod +x ${CMAKE_BINARY_DIR}/artcl.sh)

add_custom_target(tcl.tlb ALL
    DEPENDS ${TCL_SRCS})

add_custom_command(TARGET tcl.tlb PRE_BUILD
	COMMAND chmod +x ${CMAKE_BINARY_DIR}/artcl.sh
	COMMAND ${CMAKE_BINARY_DIR}/artcl.sh tcl.tlb ${TCL_SRCS}
)   

add_custom_command(OUTPUT x COMMAND echo dafuk)
add_custom_target(xrun ALL
    DEPENDS x)
