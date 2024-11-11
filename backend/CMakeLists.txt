cmake_minimum_required(VERSION 3.5)

project(backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(BACKEND_SOURCES
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
    include/database_manager.h
    src/domain.cpp
    src/transport_router.cpp 
    src/transport_catalogue.cpp 
    src/svg.cpp 
    src/request_handler.cpp 
    src/map_renderer.cpp 
    src/json_reader.cpp 
    src/json_builder.cpp 
    src/json.cpp
    src/database_manager.cpp
)
add_library(backend STATIC ${BACKEND_SOURCES})

target_include_directories(backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(backend PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(backend PRIVATE PostgreSQL::PostgreSQL)
