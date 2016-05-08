cmake_minimum_required(VERSION 3.1)

project(plugin_template)

# Forbid in-tree building
if(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


include_directories(${Qt5Core_INCLUDE_DIRS})

set(plugin_template_SRCS plugin_template.cpp main.cpp)

add_executable(plugin_template ${plugin_template_SRCS})
target_link_libraries(plugin_template ${Qt5Core_LIBRARIES})
qt5_use_modules(plugin_template Core)

install(TARGETS plugin_template RUNTIME DESTINATION bin)
