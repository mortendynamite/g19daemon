cmake_minimum_required(VERSION 3.24)

project(mpris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

set(mpris_SRC
        mpris.cpp
        mprisfetcher2.cpp
        ../../gscreen.cpp
        ../../plugininterface.hpp
)

qt_add_resources(mpris_QRC mpris.qrc)

qt_add_library(mpris SHARED ${mpris_SRC} ${mpris_QRC})
target_link_libraries(mpris ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES} Qt6::Network)

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS mpris LIBRARY DESTINATION ${PLUGINS_DIR})