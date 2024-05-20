cmake_minimum_required(VERSION 3.24)

project(keybacklight)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

set(keybacklight_SRC
        keybacklight.cpp
        ../../gscreen.cpp
        ../../plugininterface.hpp
)

qt_add_resources(keybacklight_QRC keybacklight.qrc)

qt_add_library(keybacklight SHARED ${keybacklight_SRC} ${keybacklight_QRC})
target_link_libraries(keybacklight ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES})

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS keybacklight LIBRARY DESTINATION ${PLUGINS_DIR})