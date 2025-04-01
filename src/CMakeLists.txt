cmake_minimum_required(VERSION 3.16)

project(qt-transport-catalogue VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg SvgWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg SvgWidgets)

# Явно указываем .ui файлы
# qt_wrap_ui(PROJECT_UI_FILES
#     # ${CMAKE_CURRENT_SOURCE_DIR}/view/bus_editor.ui
#     # ${CMAKE_CURRENT_SOURCE_DIR}/view/mainwindow.ui
# )

# Явно указываем исходные файлы
set(PROJECT_SOURCES
    main.cpp
    controller/bus_editor.cpp
    controller/dialog_editor.cpp
    controller/mainwindow.cpp
    controller/bus_editor.h
    controller/dialog_editor.h
    controller/mainwindow.h
    controller/mainwindow.ui
    controller/stdafx.h
    controller/bus_editor.ui
    # ${PROJECT_UI_FILES}

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-transport-catalogue
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        images/resources.qrc

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

target_link_libraries(qt-transport-catalogue PRIVATE Qt${QT_VERSION_MAJOR}::Widgets model Qt6::Svg Qt6::SvgWidgets)
target_include_directories(qt-transport-catalogue PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/model)

include(GNUInstallDirs)
install(TARGETS qt-transport-catalogue
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt-transport-catalogue)
endif()

add_subdirectory(model)
