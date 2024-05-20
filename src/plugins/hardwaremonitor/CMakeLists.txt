cmake_minimum_required(VERSION 3.24)

project(hardwaremonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network PrintSupport)
qt_standard_project_setup()

include_directories(${Qt6Core_INCLUDE_DIRS} ${Qt6Gui_INCLUDE_DIRS} ${Qt6PrintSupport_INCLUDE_DIRS} ${Qt6Network_INCLUDE_DIRS} ${LIBUSB_1_INCLUDE_DIRS})

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
        Tools/Data.cpp
        Tools/influxdb.cpp
        Tools/MonitorTool.cpp
        Tools/commandtool.cpp
        ../../gscreen.cpp
        ../../plugininterface.hpp
)

qt_add_resources(hardwaremonitor_QRC hardwaremonitor.qrc)

qt_add_library(hardwaremonitor SHARED ${hardwaremonitor_SRC} ${hardwaremonitor_QRC})
target_link_libraries(hardwaremonitor ${Qt6Core_LIBRARIES} ${Qt6PrintSupport_LIBRARIES} ${Qt6Network_LIBRARIES} ${Qt6Gui_LIBRARIES})

message("Plugins dir ${PLUGINS_DIR}")
install(TARGETS hardwaremonitor LIBRARY DESTINATION ${PLUGINS_DIR})
