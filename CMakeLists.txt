cmake_minimum_required(VERSION 3.1)

project(g19daemon)

set(VERSION_MAJOR 0 CACHE STRING "Major version" FORCE)
set(VERSION_MINOR 0 CACHE STRING "Minor version" FORCE)
set(VERSION_REVISION 9 CACHE STRING "Revison" FORCE)

set(PLUGINS_DIR lib/g19daemon/${VERSION_MAJOR}.${VERSION_MINOR}/plugins CACHE PATH "Path to installed plugins")
set(THEMES_DIR lib/g19daemon/${VERSION_MAJOR}.${VERSION_MINOR}/themes CACHE PATH "Path to installed themes")

# Forbid in-tree building
if(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

find_package(libusb-1.0 REQUIRED)
find_package(PulseAudio REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

configure_file(src/config.h.in config.h)

set(g19daemon_SRC
  src/main.cpp
  src/g19daemon.cpp
  src/g19device.cpp
  src/gscreen.cpp
)

qt5_add_resources(g19daemon_QRC src/g19daemon.qrc)

add_executable(g19daemon ${g19daemon_SRC} ${g19daemon_QRC})
target_link_libraries(g19daemon ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${LIBUSB_1_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY})

qt5_use_modules(g19daemon Core Gui)

install(TARGETS g19daemon RUNTIME DESTINATION bin)
install(PROGRAMS src/g19daemon.desktop DESTINATION share/applications)
install(FILES src/80-hid-g19.rules DESTINATION /etc/udev/rules.d)

add_subdirectory(src/plugins/helloworld)
add_subdirectory(src/plugins/keybacklight)
add_subdirectory(src/plugins/backlight)
add_subdirectory(src/plugins/pavolume)
add_subdirectory(src/plugins/mpris)

