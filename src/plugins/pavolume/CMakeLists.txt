cmake_minimum_required(VERSION 3.24)

project(pavolume)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

Qt_add_resources(pavolume_QRC pavolume.qrc)

add_library(pavolume SHARED pavolume.cpp ${pavolume_QRC})
target_link_libraries(pavolume ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})


install(TARGETS pavolume LIBRARY DESTINATION ${PLUGINS_DIR})
