cmake_minimum_required(VERSION 3.1)

project(mpris)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5DBus)
find_package(Qt5Network)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

qt5_add_resources(mpris_QRC mpris.qrc)

add_library(mpris SHARED mpris.cpp mprisfetcher2.cpp ${mpris_QRC})
target_link_libraries(mpris ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5DBus_LIBRARIES} ${Qt5Network_LIBRARIES})

qt5_use_modules(mpris Core Gui DBus)

install(TARGETS mpris LIBRARY DESTINATION ${PLUGINS_DIR})
