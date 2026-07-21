cmake_minimum_required(VERSION 3.19)
project(QLVT LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(QLVT
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    cautrucdulieu.h
    vattulogic.h vattulogic.cpp
    nhanvienlogic.h nhanvienlogic.cpp
    vattupage.h vattupage.cpp vattupage.ui
    nhanvienpage.h nhanvienpage.cpp nhanvienpage.ui
    hoadonlogic.h hoadonlogic.cpp
    thongkelogic.h thongkelogic.cpp
    hoadonpage.h hoadonpage.cpp hoadonpage.ui
    thongkepage.h thongkepage.cpp thongkepage.ui
    fileio.h fileio.cpp
    data/vattu.txt
    data/nhanvien.txt
    .gitignore
)

target_link_libraries(QLVT
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS QLVT
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QLVT
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
