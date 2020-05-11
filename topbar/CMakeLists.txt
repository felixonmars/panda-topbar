set(TARGET panda-topbar)

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

set(APPMENU_SRC
    appmenu/appmenuwidget.h
    appmenu/appmenuwidget.cpp
    appmenu/appmenudbus.h
    appmenu/appmenudbus.cpp
    appmenu/kdbusimporter.h
    appmenu/menuimporter.h
    appmenu/menuimporter.cpp
    appmenu/dbusmenutypes_p.h
)


qt5_add_dbus_adaptor(APPMENU_SRC appmenu/org.kde.kappmenu.xml
    appmenu/appmenudbus.h AppmenuDBus appmenuadaptor AppmenuAdaptor)

qt5_add_dbus_adaptor(APPMENU_SRC appmenu/com.canonical.AppMenu.Registrar.xml
    appmenu/menuimporter.h MenuImporter menuimporteradaptor MenuImporterAdaptor)

add_executable(${TARGET} ${SRCS} resources.qrc ${APPMENU_SRC})
target_link_libraries(${TARGET}
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

    ${PULSEAUDIO_LIBRARY}
    dbusmenu-qt5
)

install(TARGETS ${TARGET} DESTINATION /usr/bin)
