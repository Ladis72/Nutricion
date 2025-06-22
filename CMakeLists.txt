cmake_minimum_required(VERSION 3.19)
# Establece el codigo independiente de la posicion para todo el proyecto
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)


# ... (otras configuraciones, como add_executable)

# Enlaza tus librerias de Qt
project(Nutricion LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core  Gui Widgets Sql Charts)

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
    healtmetric.h healtmetric.cpp
    healthmetricmanager.h healthmetricmanager.cpp
    patientdetailswindow.h patientdetailswindow.cpp patientdetailswindow.ui
    resources.qrc
    addmetricdialog.h addmetricdialog.cpp addmetricdialog.ui

)

target_link_libraries(Nutricion
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
        Qt6::Widgets
        Qt6::Charts


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

# Agrega los recursos. Revisa que la ruta a resources.qrc sea correcta.
# Por ejemplo, si resources.qrc esta en la raiz de tu proyecto:
qt_add_resources(${PROJECT_NAME}_RESOURCES resources.qrc)


# Vincula los recursos compilados a tu ejecutable
target_link_libraries(${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_RESOURCES})
