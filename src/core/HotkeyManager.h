#pragma once

#include <QObject>
#include <QString>
#include <QDBusConnection>
#include <QDBusMessage>

enum class HotkeyAction {
    Fullscreen,
    Region,
    ColorPicker,
    Editor
};

class HotkeyManager : public QObject {
    Q_OBJECT

public:
    static HotkeyManager& instance();
    ~HotkeyManager() override = default;

    bool init();
    void updateHotkeys();

signals:
    void hotkeyTriggered(HotkeyAction action);

private slots:
    void onPortalActivated(const QString& actionId);

private:
    HotkeyManager();
    void requestPortalShortcuts();

    bool m_initialized = false;
};
