cmake_minimum_required(VERSION 3.16)

project(s-shot-wayland
    VERSION 1.32.0
    DESCRIPTION "Lightweight Native Wayland Screenshot & Annotation Tool for Linux"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    Network
    DBus
    Test
)

set(SOURCES
    src/main.cpp
    src/core/SettingsManager.cpp
    src/core/UpdateManager.cpp
    src/core/IconManager.cpp
    src/core/ClipboardHelper.cpp
    src/core/CrashHandler.cpp
    src/core/HotkeyManager.cpp
    src/capture/WaylandCaptureManager.cpp
    src/tray/TrayManager.cpp
    src/editor/MainWindow.cpp
    src/editor/CanvasView.cpp
    src/editor/CanvasScene.cpp
    src/editor/items/BaseAnnotationItem.cpp
    src/editor/items/PenItem.cpp
    src/editor/items/ArrowItem.cpp
    src/editor/items/ShapeItem.cpp
    src/editor/items/BadgeItem.cpp
    src/editor/items/TextItem.cpp
    src/editor/items/BlurItem.cpp
    src/editor/items/PixmapItem.cpp
    src/dialogs/SettingsDialog.cpp
    src/dialogs/AboutDialog.cpp
    resources/resources.qrc
)

add_executable(s-shot-wayland ${SOURCES})

target_include_directories(s-shot-wayland PRIVATE
    src
    src/core
    src/tray
    src/capture
    src/editor
    src/editor/items
    src/dialogs
)

target_link_libraries(s-shot-wayland PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
    Qt6::DBus
)

# Test target
enable_testing()
add_executable(test_wayland_editor
    tests/test_wayland_editor.cpp
    src/core/SettingsManager.cpp
    src/core/UpdateManager.cpp
    src/core/IconManager.cpp
    src/core/ClipboardHelper.cpp
    src/core/CrashHandler.cpp
    src/core/HotkeyManager.cpp
    src/capture/WaylandCaptureManager.cpp
    src/tray/TrayManager.cpp
    src/editor/MainWindow.cpp
    src/editor/CanvasView.cpp
    src/editor/CanvasScene.cpp
    src/editor/items/BaseAnnotationItem.cpp
    src/editor/items/PenItem.cpp
    src/editor/items/ArrowItem.cpp
    src/editor/items/ShapeItem.cpp
    src/editor/items/BadgeItem.cpp
    src/editor/items/TextItem.cpp
    src/editor/items/BlurItem.cpp
    src/editor/items/PixmapItem.cpp
    src/dialogs/SettingsDialog.cpp
    src/dialogs/AboutDialog.cpp
    resources/resources.qrc
)

target_include_directories(test_wayland_editor PRIVATE
    src
    src/core
    src/tray
    src/capture
    src/editor
    src/editor/items
    src/dialogs
)

target_link_libraries(test_wayland_editor PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Network
    Qt6::DBus
    Qt6::Test
)

add_test(NAME test_wayland_editor COMMAND test_wayland_editor)

# Installation rules
include(GNUInstallDirs)

install(TARGETS s-shot-wayland
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    resources/io.github.uri_travoski.s-shot-wayland.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES
    resources/io.github.uri_travoski.s-shot-wayland.metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
)

install(FILES
    resources/icons/io.github.uri_travoski.s-shot-wayland.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

foreach(SZ 16x16 22x22 24x24 32x32 48x48 64x64 128x128 256x256)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/hicolor/${SZ}/apps/io.github.uri_travoski.s-shot-wayland.png")
        install(FILES
            resources/icons/hicolor/${SZ}/apps/io.github.uri_travoski.s-shot-wayland.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${SZ}/apps
        )
    endif()
endforeach()

# CPack configuration
set(CPACK_PACKAGE_NAME "s-shot-wayland")
set(CPACK_PACKAGE_VENDOR "S-Shot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lightweight Native Wayland Screenshot & Annotation Tool for Linux")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/uri-travoski/S-Shot-Wayland")
set(CPACK_PACKAGE_CONTACT "support@example.com")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

# Debian packaging
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "S-Shot Developer <support@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libc6 (>= 2.34), libgcc-s1 (>= 3.0), libstdc++6 (>= 5), libqt6core6t64 (>= 6.1.2) | libqt6core6 (>= 6.1.2), libqt6gui6t64 (>= 6.1.2) | libqt6gui6 (>= 6.1.2), libqt6widgets6t64 (>= 6.1.2) | libqt6widgets6 (>= 6.1.2), libqt6network6t64 (>= 6.1.2) | libqt6network6 (>= 6.1.2), libqt6dbus6t64 (>= 6.1.2) | libqt6dbus6 (>= 6.1.2), libqt6svg6t64 (>= 6.1.2) | libqt6svg6 (>= 6.1.2)"
)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postrm"
)

# RPM packaging
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3+")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Graphics")
set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)

include(CPack)
