#pragma once

#include <QObject>
#include <QPixmap>
#include <QColor>
#include <QString>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>

class WaylandCaptureManager : public QObject {
    Q_OBJECT

public:
    static WaylandCaptureManager& instance();

    // Trigger full screen capture
    void captureFullscreen();

    // Trigger region/interactive selection capture
    void captureRegion();

    // Trigger Wayland color picker
    void pickColor();

signals:
    void screenshotReady(const QPixmap& pixmap);
    void colorPicked(const QColor& color, const QString& hex);
    void captureFailed(const QString& reason);
    void captureCancelled();

private slots:
    void onPortalResponse(uint response, const QVariantMap& results);
    void onPortalColorResponse(uint response, const QVariantMap& results);

private:
    explicit WaylandCaptureManager(QObject* parent = nullptr);
    ~WaylandCaptureManager() override = default;

    void requestPortalScreenshot(bool interactive);
    void handlePortalResponse(uint response, const QVariantMap& results, bool isColorPicker);

    // Fallback capture when portal is bypassed or in non-portal environment
    void performDirectCapture();

    int m_requestCounter = 0;
};
