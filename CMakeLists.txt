cmake_minimum_required(VERSION 3.19)
project(Nutricion LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(Nutricion
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasemanager.h databasemanager.cpp
    user.h user.cpp
    usermanager.h usermanager.cpp
)

target_link_libraries(Nutricion
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
)

include(GNUInstallDirs)

install(TARGETS Nutricion
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Nutricion
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
