cmake_minimum_required(VERSION 3.0)
set (CMAKE_C_STANDARD 99)

add_library(tk STATIC tk3d.c tkargv.c tkatom.c tkbind.c tkbitmap.c tkbutton.c tkcanvas.c tkcmds.c tkcolor.c tkconfig.c tkcursor.c tkcvarc.c tkcvbmap.c tkcvline.c tkcvpoly.c tkcvtext.c tkcvwind.c tkentry.c
  tkerror.c tkevent.c tkfont.c tkframe.c tkgc.c tkgeo.c tkget.c tkgrab.c tklist.c tkmenu.c tkmnbut.c tkmsg.c tkoption.c tkpack.c tkpixmap.c tkplace.c tkpresrv.c tkrawtcp.c tkrectov.c tkscale.c tkscrbar.c
  tkselect.c tksend.c tkshare.c tktext.c tktrig.c tktxbtre.c tktxdisp.c tktxidx.c tktxtag.c tkwindow.c tkwm.c)
#set_target_properties(tk C_EXTENSIONS off)
set_target_properties(tk PROPERTIES C_STANDARD 99 C_EXTENSIONS OFF)

add_executable(wish main.c)
target_link_libraries(wish tk tcl xstubs)

include_directories(. ../tcl /usr/include/X11)
#include_directories(. ../tcl ../../include/X11 /usr/include/X11)
#include_directories(. ../tcl ../../include/X11)
add_definitions(-DIS_LINUX -DTK_VERSION="2.3" -DUSE_XPM3)
