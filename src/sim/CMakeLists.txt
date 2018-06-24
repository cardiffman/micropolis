project(winsim C)
cmake_minimum_required(VERSION 3.0)

include(FindTCL)
if (!TCL_FOUND)
error()
endif()
MESSAGE(TCL GOOD: LIB ${TCL_LIBRARY} HEADERS: ${TCL_INCLUDE_PATH})

add_executable(winsim WIN32
		win_sim.c
        win_sprite.c
		win_stubs.c
		w_no_tk.c
		win_update.c
		win_util.c
		w_x_no_x.c
		wins_msg.c
		wins_sim.c
		wins_traf.c
		rand.c random.c
		s_alloc.c
		s_power.c
		g_ani.c wing_bigmap.c g_cam.c wing_map.c wing_setup.c wing_smmaps.c
		rand.c random.c
		wins_disast.c s_eval.c wins_fileio.c wins_gen.c wins_init.c wins_msg.c s_power.c s_scan.c
        wins_sim.c wins_traf.c s_zone.c
        #sim.c
        win_budget.c w_cam.c w_con.c
        win_keys.c
        w_net.c
        #w_piem.c
	    #w_sim.c
	    win_cmds.c
		win_date.c win_editor.c win_inter.c win_map.c win_preserv.c win_sim_cmds.c
		win_graph.c
        win_eval.c
		win_sound.c
		win_sprite.c
		win_stubs.c
		win_tool.c
		win_update.c
		win_util.c
        w_resrc.c
		w_no_tk.c
		w_x_no_x.c
		)
target_link_libraries(winsim gdi32 user32 ${TCL_LIBRARY})
set_target_properties(winsim PROPERTIES C_EXTENSIONS OFF)
add_definitions(-DIS_LINUX -Werror -DUNICODE -D_UNICODE -mwindows)
include_directories(headers ${TCL_INCLUDE_PATH})
if (false)
add_executable(sim
		g_ani.c g_bigmap.c g_cam.c g_map.c g_setup.c g_smmaps.c
		rand.c random.c
		s_alloc.c s_disast.c s_eval.c s_fileio.c s_gen.c s_init.c s_msg.c s_power.c s_scan.c
        s_sim.c s_traf.c s_zone.c
        sim.c
        w_budget.c w_cam.c w_con.c
        w_date.c
        w_editor.c
        w_eval.c
        w_graph.c
        w_inter.c
        w_keys.c
        w_map.c
        w_net.c
        w_piem.c
        w_resrc.c
	    w_sim.c
		w_sound.c w_sprite.c w_stubs.c
		w_tool.c w_update.c w_util.c
		w_tk.c
		w_x.c
		)
target_link_libraries(sim xstubs tcl tk Xpm tclx)
#target_link_libraries(sim xstubs tcl)
#target_link_libraries(sim xstubs)
target_link_libraries(sim xstubs)
set(extra w_print.c)
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
#include_directories(headers ../tclx/src ../tcl ../tk)
include_directories(headers ../tcl)


#include_directories(../../include /usr/include/X11 headers ../tclx/src ../tcl ../tk)
endif()
