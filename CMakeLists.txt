cmake_minimum_required(VERSION 3.5)

project(qt-transport-catalogue VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Connect Qt Modules (Widgets и Sql)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# Connect PostgreSQL
set(PostgreSQL_LIBRARY "C:/dev/PostgreSQL/17/lib/libpq.lib")
set(PostgreSQL_INCLUDE_DIR "C:/dev/PostgreSQL/17/include")
find_package(PostgreSQL REQUIRED)

add_subdirectory(backend)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/qt/ui")

set(PROJECT_SOURCES
    qt/src/main.cpp
    qt/src/mainwindow.cpp
    qt/include/mainwindow.h
    qt/ui/mainwindow.ui
    qt/src/bus_editor.cpp
    qt/include/bus_editor.h
    qt/ui/bus_editor.ui
    qt/resources/resources.qrc
    qt/include/dialog_editor.h qt/src/dialog_editor.cpp qt/ui/dialog_editor.ui
    qt/include/stdafx.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-transport-catalogue
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        # qt/include/dialog_editor.h qt/src/dialog_editor.cpp qt/ui/dialog_editor.ui
        # qt/include/stdafx.h

    )
else()
    if(ANDROID)
        add_library(qt-transport-catalogue SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qt-transport-catalogue
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Подключаем библиотеки Qt и PostgreSQL
target_link_libraries(qt-transport-catalogue PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(qt-transport-catalogue PRIVATE PostgreSQL::PostgreSQL)

# Подключаем библиотеку Qt SVG и Qt SvgWidgets
find_package(Qt6 COMPONENTS Svg SvgWidgets REQUIRED)
target_link_libraries(qt-transport-catalogue PRIVATE Qt6::Svg Qt6::SvgWidgets)
target_include_directories(qt-transport-catalogue PRIVATE ${CMAKE_BINARY_DIR})

# Связываем с backend
target_include_directories(qt-transport-catalogue PRIVATE
    ${CMAKE_SOURCE_DIR}/qt/include
    ${CMAKE_SOURCE_DIR}/qt/src
    ${CMAKE_SOURCE_DIR}/qt/
    ${CMAKE_SOURCE_DIR}/backend/include
)
target_link_libraries(qt-transport-catalogue PRIVATE backend)

set_target_properties(qt-transport-catalogue PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS qt-transport-catalogue
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-transport-catalogue)
endif()
