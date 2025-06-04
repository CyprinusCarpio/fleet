cmake_minimum_required(VERSION 3.15.0)

set (CMAKE_CXX_STANDARD 11)

project(Fleet VERSION 0.1.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(FLEET_BUILD_DEMO ON)
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY bin/${CMAKE_BUILD_TYPE})
else()
    set(FLEET_BUILD_DEMO OFF)
endif()

find_package(FLTK CONFIG REQUIRED)

set(FLE_CPP_FILES
	src/Fle_Flat_Button.cpp
	src/Fle_Dock_Group.cpp 
	src/Fle_Dock_Host.cpp
	src/Fle_Schemes.cpp
	src/Fle_Orientable_Flex.cpp
	src/Fle_Toolbar.cpp
	src/Fle_Listview_Item.cpp
	src/Fle_Listview.cpp
	src/Fle_Colors.cpp
	src/Fle_Accordion.cpp
	src/Fle_Accordion_Group.cpp
	src/Fle_Stack.cpp
)

set(FLE_HPP_FILES
	include/FLE/Fle_Flat_Button.hpp
	include/FLE/Fle_Dock_Group.hpp
	include/FLE/Fle_Dock_Host.hpp
	include/FLE/Fle_Schemes.hpp
	include/FLE/Fle_Orientable_Flex.hpp
	include/FLE/Fle_Toolbar.hpp
	include/FLE/Fle_Listview_Item.hpp
	include/FLE/Fle_Listview.hpp
	include/FLE/Fle_Colors.hpp
	include/FLE/Fle_Accordion.hpp
	include/FLE/Fle_Accordion_Group.hpp
	include/FLE/Fle_Stack.hpp
)

add_library(Fleet ${FLE_CPP_FILES})
target_include_directories(Fleet PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(Fleet PUBLIC fltk)

if(FLEET_BUILD_DEMO)
	add_executable(FleetDemo ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
	target_include_directories(FleetDemo PRIVATE ${CMAKE_SOURCE_DIR}/include )
	target_link_libraries(FleetDemo PRIVATE fltk Fleet)
endif()
