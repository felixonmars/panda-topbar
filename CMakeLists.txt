cmake_minimum_required(VERSION 3.0)

project(panda-topbar)
set(PROJECT_VERSION "1.0")
set(EXEC_NAME panda-topbar)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_FLAGS "-g -Wall")

set(SRCS
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainpanel.h
    mainpanel.cpp
    datetimewidget.h
    datetimewidget.cpp

    tray/trayicon.cpp
    tray/trayicon.h
    tray/traywidget.h
    tray/traywidget.cpp
    tray/xfitman.h
    tray/xfitman.cpp

    volume/volumewidget.cpp
    volume/volumewidget.h
)

find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(KF5DBusAddons REQUIRED)
find_package(KF5WindowSystem REQUIRED)

# find_package(XCB REQUIRED COMPONENTS xcb xcb-util xcb-damage)
find_package(X11 REQUIRED)
find_package(PulseAudio)

pkg_check_modules(XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules(XDAMAGE REQUIRED xdamage)
pkg_check_modules(XRENDER REQUIRED xrender)
pkg_check_modules(XCB_EWMH REQUIRED xcb-ewmh x11)

#target_include_directories(${PROJECT_NAME} PUBLIC ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                               ${XCB_EWMH_INCLUDE_DIRS}
#                                               ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
#                                               ${PROJECT_BINARY_DIR})

add_executable(${EXEC_NAME} ${SRCS})
target_link_libraries(${EXEC_NAME}
    Qt5::DBus
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::X11Extras
    KF5::DBusAddons
    KF5::WindowSystem

    ${X11_LIBRARIES}
    ${XCOMPOSITE_LDFLAGS}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XCB_LIBRARIES}

    ${PULSEAUDIO_LIBRARY}
)