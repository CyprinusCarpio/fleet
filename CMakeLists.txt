cmake_minimum_required(VERSION 3.15.0)

set (CMAKE_CXX_STANDARD 11)

project(FleetDemo VERSION 0.1.0)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

find_package(FLTK CONFIG REQUIRED)

add_executable(FleetDemo main.cpp 
	src/Fle_Flat_Button.cpp
	src/Fle_Dock_Group.cpp 
	src/Fle_Dock_Host.cpp
)

file(GLOB FLE_HEADER_FILES "${CMAKE_SOURCE_DIR}/include/FLE/*.hpp")
target_sources(FleetDemo PRIVATE
	${FLE_HEADER_FILES}
)

target_include_directories(FleetDemo PUBLIC ${CMAKE_SOURCE_DIR}/include )

target_link_libraries(FleetDemo PRIVATE fltk)

