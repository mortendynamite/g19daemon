cmake_minimum_required(VERSION 3.1)

project(hardwaremonitor)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5PrintSupport)

include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS} )

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -ggdb")

set(hardwaremonitor_SRC
Defines.cpp
    hardwaremonitor.cpp
  external/QCustomPlot/qcustomplot.cpp
  HwaSettings.cpp
  Screen/StartScreen.cpp
  Screen/NormalScreen.cpp
  Screen/LegendScreen.cpp
  Screen/GraphScreen.cpp
  Screen/Screen.cpp
  hardwaremonitor.cpp
)

qt5_add_resources(hardwaremonitor_QRC hardwaremonitor.qrc)

add_library(hardwaremonitor SHARED ${hardwaremonitor_SRC} ${hardwaremonitor_QRC})
target_link_libraries(hardwaremonitor ${Qt5Core_LIBRARIES} ${Qt5PrintSupport_LIBRARIES} ${Qt5Gui_LIBRARIES})

qt5_use_modules(hardwaremonitor Core Gui)

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS hardwaremonitor LIBRARY DESTINATION ${PLUGINS_DIR})
