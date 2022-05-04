cmake_minimum_required(VERSION 3.1)

project(TransportCatalogue VERSION 1.0)
set (PROJECT TransportCatalogue) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SRC_LIST src/main.cpp
		src/domain.cpp
		src/geo.cpp
		src/svg.cpp
		src/request_handler.cpp
		src/json.cpp
		src/json_builder.cpp
		src/json_reader.cpp
		src/transport_catalogue.cpp
		src/map_renderer.cpp)

include_directories("include/")

set (HEADERS
	include/domain.h
	include/geo.h
	include/svg.h
	include/request_handler.h
	include/json.h
	include/json_builder.h
	include/json_reader.h
	include/transport_catalogue.h
	include/map_renderer.h)

add_executable(${PROJECT} ${HEADERS} ${SRC_LIST})