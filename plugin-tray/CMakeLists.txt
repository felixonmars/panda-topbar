set(PLUGIN "systemtray")
project(${PLUGIN})

set(SRCS
    trayicon.h
    trayicon.cpp
    traywidget.h
    traywidget.cpp
    xfitman.h
    xfitman.cpp
    plugin.h
)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN} SHARED ${SRCS})

target_link_libraries(${PLUGIN} PRIVATE
    Qt5::Widgets
    Qt5::X11Extras
    ${X11_LIBRARIES}
    ${XCOMPOSITE_LDFLAGS}
    ${XDAMAGE_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XCB_LIBRARIES}
    ${XCB_EWMH_LIBRARIES}
)
target_include_directories(${PLUGIN} PUBLIC ../interfaces)

install(TARGETS ${PLUGIN} LIBRARY DESTINATION /usr/lib/panda-topbar/plugins)