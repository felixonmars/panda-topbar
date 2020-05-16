set(PLUGIN "globalmenu")
project(${PLUGIN})

set(SRCS
    appmenuwidget.h
    appmenuwidget.cpp
    appmenudbus.h
    appmenudbus.cpp
    kdbusimporter.h
    menuimporter.h
    menuimporter.cpp
    dbusmenutypes_p.h
    plugin.h
)

qt5_add_dbus_adaptor(SRCS org.kde.kappmenu.xml
    appmenudbus.h AppmenuDBus appmenuadaptor AppmenuAdaptor)

qt5_add_dbus_adaptor(SRCS com.canonical.AppMenu.Registrar.xml
    menuimporter.h MenuImporter menuimporteradaptor MenuImporterAdaptor)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN} SHARED ${SRCS})

target_link_libraries(${PLUGIN} PRIVATE
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
    ${XCB_EWMH_LIBRARIES}
    dbusmenu-qt5
)
target_include_directories(${PLUGIN} PUBLIC ../interfaces)

install(TARGETS ${PLUGIN} LIBRARY DESTINATION /usr/lib/panda-topbar/plugins)