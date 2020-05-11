cmake_minimum_required(VERSION 3.0)

project(panda-topbar)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(PkgConfig REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5X11Extras REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(KF5DBusAddons REQUIRED)
find_package(KF5WindowSystem REQUIRED)
find_package(dbusmenu-qt5 REQUIRED)
find_package(XCB REQUIRED COMPONENTS xcb xcb-util xcb-damage)
find_package(X11 REQUIRED)
find_package(PulseAudio)

pkg_check_modules(XCOMPOSITE REQUIRED xcomposite)
pkg_check_modules(XDAMAGE REQUIRED xdamage)
pkg_check_modules(XRENDER REQUIRED xrender)
pkg_check_modules(XCB_EWMH REQUIRED xcb-ewmh x11)

file(GLOB INTERFACES "interfaces/*.h")

add_subdirectory("topbar")
add_subdirectory("plugin-datetime")
add_subdirectory("plugin-statusnotifier")

install(FILES ${INTERFACES} DESTINATION /usr/include/panda-topbar)