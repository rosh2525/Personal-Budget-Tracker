cmake_minimum_required(VERSION 3.20)
project(Personal_Budget_Tracker C)

set(CMAKE_C_STANDARD 11)

# Find GTK
find_package(PkgConfig REQUIRED)
pkg_search_module(GTK REQUIRED gtk+-3.0)

# Add executable
add_executable(Personal_Budget_Tracker main.c gui.c)

# Include GTK headers
target_include_directories(Personal_Budget_Tracker PRIVATE ${GTK_INCLUDE_DIRS})

# Link GTK libraries
target_link_libraries(Personal_Budget_Tracker PRIVATE ${GTK_LIBRARIES})
