project(winsim C)
cmake_minimum_required(VERSION 3.0)

include(FindTCL)
if (!TCL_FOUND)
error()
endif()
MESSAGE(TCL GOOD: LIB ${TCL_LIBRARY} HEADERS: ${TCL_INCLUDE_PATH})

add_executable(winsim WIN32
		win_sim.c
		s_alloc.c
		wing_map.c
		wing_smmaps.c
		wins_sim.c
		rand.c
        s_zone.c
		wins_traf.c
		s_power.c
		wins_msg.c
		s_eval.c
        win_eval.c
        win_graph.c
        win_sprite.c
        s_scan.c
        win_sound.c
        ###
        win_tool.c
        win_ink.c
        win_budget.c
        w_con.c
        win_update.c
        wins_msg.c
        win_util.c
        wins_disast.c
        wins_fileio.c
        w_resrc.c
        wins_init.c
        win_map.c
        win_editor.c
        wing_setup.c
        ####
        w_no_tk.c
        w_x_no_x.c
        win_stubs.c
        win_keys.c
        wins_gen.c
		)
#target_link_libraries(winsim gdi32 user32 ${TCL_LIBRARY})
target_link_libraries(winsim XpmFileData gdi32 user32)
set_target_properties(winsim PROPERTIES C_EXTENSIONS OFF)
add_definitions(-DIS_LINUX -Werror -DUNICODE -D_UNICODE -mwindows)
include_directories(headers ../XpmFileData ${TCL_INCLUDE_PATH})
