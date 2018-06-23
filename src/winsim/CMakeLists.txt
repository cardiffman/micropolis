project(winsim C)
cmake_minimum_required(VERSION 3.0)

include(FindTCL)
if (!TCL_FOUND)
error()
endif()
MESSAGE(TCL GOOD: LIB ${TCL_LIBRARY} HEADERS: ${TCL_INCLUDE_PATH})

add_executable(winsim WIN32
		win_sim.c
		rand.c 
		s_power.c 
        win_eval.c 
		wins_msg.c
		wins_fileio.c 
		win_graph.c
		wins_init.c 
        win_budget.c 
		wins_sim.c
		win_util.c 
		win_stubs.c 
        w_resrc.c 
		s_alloc.c 
		w_no_tk.c
		win_update.c 
        win_sprite.c
		s_scan.c 
        s_zone.c 
        wins_traf.c 
		win_sound.c 
		win_tool.c 
		w_x_no_x.c
		wing_setup.c 
        w_con.c 
		wins_disast.c 
		s_eval.c 
        win_keys.c 
		win_map.c 
		wing_map.c 
		wing_smmaps.c 
		wins_gen.c 
		win_editor.c 
		#random.c 
		#s_power.c
		#g_ani.c wing_bigmap.c g_cam.c 
		#rand.c random.c 
		#wins_msg.c 
        #w_cam.c 
        #w_net.c 
	    #win_cmds.c
		#win_date.c 
		#win_inter.c 
		#win_preserv.c 
		#win_sim_cmds.c
		#win_sprite.c 
		#win_stubs.c 
		#win_update.c 
		#win_util.c 
		)
#target_link_libraries(winsim gdi32 user32 ${TCL_LIBRARY})
target_link_libraries(winsim gdi32 user32)
set_target_properties(winsim PROPERTIES C_EXTENSIONS OFF)
add_definitions(-DIS_LINUX -Werror -DUNICODE -D_UNICODE -mwindows)
include_directories(headers ${TCL_INCLUDE_PATH})
