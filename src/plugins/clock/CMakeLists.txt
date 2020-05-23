cmake_minimum_required(VERSION 3.1)

project(clock)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt5Core)
find_package(Qt5Gui)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

set(clock_SRC
  clock.cpp
)

qt5_add_resources(clock_QRC clock.qrc)

add_library(clock SHARED ${clock_SRC} ${clock_QRC})
target_link_libraries(clock ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES})

qt5_use_modules(clock Core Gui)

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS clock LIBRARY DESTINATION ${PLUGINS_DIR})
