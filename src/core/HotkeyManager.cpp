#include "HotkeyManager.h"
#include "SettingsManager.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDebug>

HotkeyManager& HotkeyManager::instance() {
    static HotkeyManager s_instance;
    return s_instance;
}

HotkeyManager::HotkeyManager() {
}

bool HotkeyManager::init() {
    if (m_initialized) return true;
    m_initialized = true;
    requestPortalShortcuts();
    return true;
}

void HotkeyManager::updateHotkeys() {
    requestPortalShortcuts();
}

void HotkeyManager::requestPortalShortcuts() {
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) return;

    // org.freedesktop.portal.GlobalShortcuts integration
    QDBusMessage msg = QDBusMessage::createMethodCall(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.GlobalShortcuts",
        "CreateSession"
    );

    QVariantMap options;
    options["handle_token"] = "sshot_gs_session";
    options["session_handle_token"] = "sshot_gs_token";
    msg << options;

    bus.asyncCall(msg);
}

void HotkeyManager::onPortalActivated(const QString& actionId) {
    if (actionId == "fullscreen") {
        emit hotkeyTriggered(HotkeyAction::Fullscreen);
    } else if (actionId == "region") {
        emit hotkeyTriggered(HotkeyAction::Region);
    } else if (actionId == "colorpicker") {
        emit hotkeyTriggered(HotkeyAction::ColorPicker);
    } else if (actionId == "editor") {
        emit hotkeyTriggered(HotkeyAction::Editor);
    }
}
