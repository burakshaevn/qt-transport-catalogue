cmake_minimum_required(VERSION 3.5)

project(backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем директорию с заголовочными файлами
include_directories(include)

# Подключаем файлы database_manager.cpp и .h
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
    ../database_manager.h
    src/transport_router.cpp 
    src/transport_catalogue.cpp 
    src/svg.cpp 
    src/request_handler.cpp 
    src/map_renderer.cpp 
    src/json_reader.cpp 
    src/json_builder.cpp 
    src/json.cpp
    ../database_manager.cpp   
)

add_library(backend STATIC ${BACKEND_SOURCES})

# Подключаем библиотеки Qt и PostgreSQL для работы с database_manager
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

target_link_libraries(backend PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(backend PRIVATE PostgreSQL::PostgreSQL)

# Подключаем библиотеку PostgreSQL (если требуется)
target_link_libraries(backend PRIVATE PostgreSQL::PostgreSQL)
