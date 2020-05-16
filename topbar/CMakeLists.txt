set(TARGET panda-topbar)

set(SRCS
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainpanel.h
    mainpanel.cpp
    pluginmanager.h
    pluginmanager.cpp

    volume/volumewidget.cpp
    volume/volumewidget.h
)

add_executable(${TARGET} ${SRCS} resources.qrc)
target_link_libraries(${TARGET}
    Qt5::DBus
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::X11Extras
    KF5::DBusAddons
    KF5::WindowSystem

    ${PULSEAUDIO_LIBRARY}
    dbusmenu-qt5
)

install(TARGETS ${TARGET} DESTINATION /usr/bin)
