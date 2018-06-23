project(myxpm C)

add_library(XpmFileData STATIC XpmFileData.c)

# Executable has WinMain, use cygstart to pass argument
# Or start from gdb
add_executable(xpmtest xpmtest.c)

target_link_libraries(xpmtest XpmFileData gdi32)
set_target_properties(xpmtest PROPERTIES C_EXTENSIONS OFF)
add_definitions(-DIS_LINUX -Werror -DUNICODE -D_UNICODE -mwindows)
