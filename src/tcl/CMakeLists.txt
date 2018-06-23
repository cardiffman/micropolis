cmake_minimum_required(VERSION 3.0)
project(tcl C)
add_library(tcl STATIC panic.c regexp.c tclassem.c 
tclbasic.c tclckall.c tclcmdah.c tclcmdil.c tclcmdmz.c 
tclenv.c tclexpr.c tclget.c tclglob.c tclhash.c tclhist.c 
tclparse.c tclproc.c 
#tcltest.c # may contain main()!! 
tclunxaz.c tclunxst.c tclunxut.c tclutil.c tclvar.c)
add_definitions(-DTCL_LIBRARY="/usr/local/lib/tcl" -DIS_LINUX)

add_executable(tcltest tcltest.c)
target_link_libraries(tcltest tcl)
