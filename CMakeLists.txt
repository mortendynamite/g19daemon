cmake_minimum_required(VERSION 3.24)

project(g19daemon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CPACK_RPM_PACKAGE_SUMMARY "G19daemon provides multiple virtual screens for the LCD on the Logitech G19 Keyboar")
set(CPACK_RPM_PACKAGE_NAME "g19deamon")
set(CPACK_RPM_PACKAGE_DESCRIPTION "G19daemon provides multiple virtual screens for the LCD on the Logitech G19 Keyboar")
include(CPack)

set(VERSION_MAJOR 0 CACHE STRING "Major version" FORCE)
set(VERSION_MINOR 1 CACHE STRING "Minor version" FORCE)
set(VERSION_REVISION 2 CACHE STRING "Revison" FORCE)

set(PLUGINS_DIR lib/g19daemon/${VERSION_MAJOR}.${VERSION_MINOR}/plugins CACHE PATH "Path to installed plugins")
set(THEMES_DIR lib/g19daemon/${VERSION_MAJOR}.${VERSION_MINOR}/themes CACHE PATH "Path to installed themes")

# Forbid in-tree building
if(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})
      message(STATUS "Please do an out-of-tree build:")
      message(STATUS "rm -f CMakeCache.txt && mkdir build && cd build; cmake .. && make")
      message(FATAL_ERROR "In-tree-build detected!")
endif(${CMAKE_SOURCE_DIR} MATCHES ${CMAKE_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -ggdb")

add_compile_definitions(QT_DISABLE_DEPRECATED_UP_TO=0x050F00)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets DBus)
qt_standard_project_setup()

find_package(libusb-1.0 REQUIRED)
find_package(PulseAudio REQUIRED)

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${Qt6Widgets_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

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

qt_add_resources(g19daemon_QRC src/g19daemon.qrc)

set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
# If using Qt6, override DEFAULT_MAJOR_VERSION
set(QT_DEFAULT_MAJOR_VERSION 6 CACHE STRING "Qt version to use, defaults to 6")

add_subdirectory(./src/singleapplication)

add_executable(g19daemon ${g19daemon_SRC} ${g19daemon_QRC})
target_link_libraries(g19daemon ${Qt6Core_LIBRARIES} ${Qt6Gui_LIBRARIES} ${Qt6Widgets_LIBRARIES} ${LIBUSB_1_LIBRARIES} ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_MAINLOOP_LIBRARY} SingleApplication::SingleApplication)

#qt5_use_modules(g19daemon Core Gui)

install(TARGETS g19daemon RUNTIME DESTINATION bin)
install(PROGRAMS src/g19daemon.desktop DESTINATION share/applications)
install(FILES helper/80-hid-g19.rules DESTINATION /etc/udev/rules.d)
install(FILES src/images/g19daemon.png DESTINATION /usr/share/icons)

add_subdirectory(src/plugins/helloworld)
add_subdirectory(src/plugins/keybacklight)
add_subdirectory(src/plugins/backlight)
add_subdirectory(src/plugins/pavolume)
add_subdirectory(src/plugins/mpris)
add_subdirectory(src/plugins/clock)
#add_subdirectory(src/plugins/hardwaremonitor)

#
#target_link_libraries(${PROJECT_NAME} SingleApplication::SingleApplication)

