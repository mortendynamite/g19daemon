cmake_minimum_required(VERSION 3.24)

project(helloworld)

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

set(helloworld_SRC
  helloworld.cpp
  ../../gscreen.hpp
        ../../gscreen.cpp
  ../../plugininterface.hpp
)

qt_add_resources(helloworld_QRC helloworld.qrc)

qt_add_library(helloworld SHARED ${helloworld_SRC} ${helloworld_QRC})
target_link_libraries(helloworld ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES})

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS helloworld LIBRARY DESTINATION ${PLUGINS_DIR})
