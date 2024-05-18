cmake_minimum_required(VERSION 3.24)

project(clock)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

set(clock_SRC
        clock.cpp
        ../../gscreen.cpp
        ../../plugininterface.hpp
)

Qt_add_resources(clock_QRC clock.qrc)

qt_add_library(clock SHARED ${clock_SRC} ${clock_QRC})
target_link_libraries(clock ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES})

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS clock LIBRARY DESTINATION ${PLUGINS_DIR})
