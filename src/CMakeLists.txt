cmake_minimum_required(VERSION 3.0)
project(micropolis C)

add_subdirectory(winsim)
add_subdirectory(XpmFileData)
if (false)
add_subdirectory(sim)
add_subdirectory(xstubs)
add_subdirectory(tcl)
add_subdirectory(tk)
#add_subdirectory(tclx)
endif()