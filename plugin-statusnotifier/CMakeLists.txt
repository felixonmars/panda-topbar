set(PLUGIN "statusnotifier")
project(${PLUGIN})

find_package(dbusmenu-qt5 REQUIRED)

set(SRCS
    statusnotifier.h
    statusnotifier.cpp
)

#qt5_add_dbus_adaptor(DBUS_SOURCES
#    org.kde.StatusNotifierItem.xml
#    statusnotifieriteminterface.h
#    StatusNotifierItemInterface
#)

#set_source_files_properties(${DBUS_SOURCES} PROPERTIES SKIP_AUTOGEN ON)

add_definitions("${QT_DEFINITIONS} -DQT_PLUGIN")
add_library(${PLUGIN} SHARED ${SRCS})

target_link_libraries(${PLUGIN} PRIVATE
    ${Qt5Widgets_LIBRARIES}
    dbusmenu-qt5
)
target_include_directories(${PLUGIN} PUBLIC ../interfaces)

install(TARGETS ${PLUGIN} LIBRARY DESTINATION /usr/lib/panda-topbar/plugins)
