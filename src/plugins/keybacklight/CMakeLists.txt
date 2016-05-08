cmake_minimum_required(VERSION 3.1)

project(keybacklight)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(PulseAudio)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS} ${PULSEAUDIO_INCLUDE_DIR})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

qt5_add_resources(keybacklight_QRC keybacklight.qrc)

add_library(keybacklight MODULE keybacklight.cpp ${keybacklight_QRC})
target_link_libraries(keybacklight LINK_INTERFACE_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})

qt5_use_modules(keybacklight Core Gui)

install(TARGETS keybacklight LIBRARY DESTINATION ${PLUGINS_DIR})
