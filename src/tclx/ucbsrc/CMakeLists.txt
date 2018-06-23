cmake_minimum_required(VERSION 3.6)
project(ucbsrc C)

set (TCL_UCB_DIR ${CMAKE_SOURCE_DIR}/../../tcl)

# tclbasic.c: ../$(TCL_UCB_DIR)/tclbasic.c
#	rm -f tclbasic.c
#	sed -f tclbasic.sed <$(TCL_UCB_DIR)/tclbasic.c >tclbasic.c

#tclexpr.c: ../$(TCL_UCB_DIR)/tclexpr.c
#	rm -f tclexpr.c
#	sed -f tclexpr.sed <$(TCL_UCB_DIR)/tclexpr.c >tclexpr.c

#add_custom_command(TARGET tcl.tlb PRE_BUILD
#	COMMAND chmod +x ${CMAKE_BINARY_DIR}/artcl.sh
#	COMMAND ${CMAKE_BINARY_DIR}/artcl.sh tcl.tlb ${TCL_SRCS}
#)   

add_custom_target(tclexpr.c ALL
    DEPENDS ../${TCL_UCB_DIR}/tclexpr.c)
    
add_custom_target(tclbasic.c ALL
    DEPENDS ../${TCL_UCB_DIR}/tclbasic.c)
    
add_custom_command(TARGET tclbasic.c PRE_BUILD
	COMMAND rm -f tclbasic.c
	COMMAND sed -f ${CMAKE_SOURCE_DIR}/tclbasic.sed <${TCL_UCB_DIR}/tclbasic.c >tclbasic.c)

add_custom_command(TARGET tclexpr.c PRE_BUILD
	COMMAND rm -f tclexpr.c
	COMMAND sed -f ${CMAKE_SOURCE_DIR}/tclexpr.sed <${TCL_UCB_DIR}/tclexpr.c >tclexpr.c)

#made.tmp: $(OBJS)
#	-$(AR) d  ../libtcl.a $(OBJS)
#	$(AR)  cr ../libtcl.a $(OBJS)
#	touch made.tmp

add_custom_target(made.tmp ALL
    DEPENDS ${CMAKE_BINARY_DIR}/tclbasic.o tclexpr.o)

add_custom_target(CMAKE_BINARY_DIR}/tclbasic.o ALL
    DEPENDS tclbasic.c)

add_custom_command(TARGET made.tmp PRE_BUILD
	COMMAND $(AR) d  ../libtcl.a tclbasic.o tclexpr.o
	COMMAND $(AR) cr ../libtcl.a tclbasic.o tclexpr.o
	touch made.tmp)
	