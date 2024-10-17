cmake_minimum_required(VERSION 3.5)

project(backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(backend STATIC
    include/domain.h 
    include/geo.h 
    include/graph.h 
    include/json.h 
    include/json_builder.h 
    include/json_reader.h 
    include/map_renderer.h 
    include/ranges.h 
    include/request_handler.h 
    include/router.h 
    include/svg.h 
    include/transport_catalogue.h 
    include/transport_router.h
    src/transport_router.cpp 
    src/transport_catalogue.cpp 
    src/svg.cpp 
    src/request_handler.cpp 
    src/map_renderer.cpp 
    src/json_reader.cpp 
    src/json_builder.cpp 
    src/json.cpp
)
target_link_libraries(backend PRIVATE PostgreSQL::PostgreSQL)
