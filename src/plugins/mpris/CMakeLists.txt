cmake_minimum_required(VERSION 3.24)

project(mpris)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui DBus Network)
qt_standard_project_setup()

#find_package(Qt6Core)
#find_package(Qt6Gui)
#find_package(Qt6DBus)
#find_package(Qt6Network)

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${Qt6DBus_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

Qt_add_resources(mpris_QRC mpris.qrc)

add_library(mpris SHARED mpris.cpp mprisfetcher2.cpp ${mpris_QRC})
target_link_libraries(mpris ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES} ${Qt6DBus_LIBRARIES} ${Qt6Network_LIBRARIES})


install(TARGETS mpris LIBRARY DESTINATION ${PLUGINS_DIR})
