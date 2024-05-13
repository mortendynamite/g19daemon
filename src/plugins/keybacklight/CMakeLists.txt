cmake_minimum_required(VERSION 3.24)

project(keybacklight)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(PulseAudio)
find_package(Qt6 REQUIRED COMPONENTS Core Gui)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS} ${PULSEAUDIO_INCLUDE_DIR})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

Qt_add_resources(keybacklight_QRC keybacklight.qrc)

add_library(keybacklight MODULE keybacklight.cpp ${keybacklight_QRC})
target_link_libraries(keybacklight LINK_INTERFACE_LIBRARIES ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})


install(TARGETS keybacklight LIBRARY DESTINATION ${PLUGINS_DIR})
