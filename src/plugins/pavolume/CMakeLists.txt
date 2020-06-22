cmake_minimum_required(VERSION 3.1)

project(pavolume)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt5Core)
find_package(Qt5Gui)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

qt5_add_resources(pavolume_QRC pavolume.qrc)

add_library(pavolume SHARED pavolume.cpp ${pavolume_QRC})
target_link_libraries(pavolume ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})

qt5_use_modules(pavolume Core Gui)

install(TARGETS pavolume LIBRARY DESTINATION ${PLUGINS_DIR})
