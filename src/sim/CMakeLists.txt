cmake_minimum_required(VERSION 3.0)
add_executable(sim g_ani.c g_bigmap.c g_cam.c g_map.c g_setup.c g_smmaps.c rand.c random.c s_alloc.c s_disast.c s_eval.c s_fileio.c s_gen.c s_init.c s_msg.c s_power.c s_scan.c 
              s_sim.c s_traf.c s_zone.c sim.c w_budget.c w_cam.c w_con.c w_date.c w_editor.c w_eval.c w_graph.c w_inter.c w_keys.c w_map.c w_net.c w_piem.c w_resrc.c 
			  w_sim.c w_sound.c w_sprite.c w_stubs.c w_tk.c w_tool.c w_update.c w_util.c w_x.c)
add_definitions(-DIS_LINUX)
set(extra w_print.c)
target_link_libraries(sim xstubs tcl tk)
#target_link_libraries(sim xstubs tcl)
# u_int issue but no xlocale issue
#set_target_properties(sim PROPERTIES C_STANDARD 99 C_EXTENSIONS OFF)
# comment issue
#set_target_properties(sim PROPERTIES C_STANDARD 90 C_EXTENSIONS OFF)
# xlocale issue
#set_target_properties(sim PROPERTIES C_STANDARD 90)
# xlocal issue if no definition of C specification is given.
set_target_properties(sim PROPERTIES C_EXTENSIONS OFF)

#Xlocale issue with default compiler
#include_directories(headers ../tclx/src ../tcl ../tk /usr/include/X11)
include_directories(headers ../tclx/src ../tcl ../tk)


#include_directories(../../include /usr/include/X11 headers ../tclx/src ../tcl ../tk)
