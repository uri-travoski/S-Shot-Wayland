#include "WaylandCaptureManager.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusPendingReply>
#include <QUrl>
#include <QFile>
#include <QImageReader>
#include <QDateTime>
#include <QRandomGenerator>
#include <QDebug>

static const QString PORTAL_SERVICE = "org.freedesktop.portal.Desktop";
static const QString PORTAL_PATH = "/org/freedesktop/portal/desktop";
static const QString PORTAL_INTERFACE = "org.freedesktop.portal.Screenshot";
static const QString REQUEST_INTERFACE = "org.freedesktop.portal.Request";

WaylandCaptureManager& WaylandCaptureManager::instance() {
    static WaylandCaptureManager s_instance;
    return s_instance;
}

WaylandCaptureManager::WaylandCaptureManager(QObject* parent)
    : QObject(parent)
{
}

void WaylandCaptureManager::captureFullscreen() {
    requestPortalScreenshot(false);
}

void WaylandCaptureManager::captureRegion() {
    requestPortalScreenshot(true);
}

void WaylandCaptureManager::requestPortalScreenshot(bool interactive) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        performDirectCapture();
        return;
    }

    QString token = QString("sshot_%1_%2").arg(QDateTime::currentMSecsSinceEpoch())
                                         .arg(QRandomGenerator::global()->generate());

    QVariantMap options;
    options["handle_token"] = token;
    options["interactive"] = interactive;
    options["modal"] = false;

    QDBusMessage message = QDBusMessage::createMethodCall(
        PORTAL_SERVICE, PORTAL_PATH, PORTAL_INTERFACE, "Screenshot"
    );
    message << QString("") << options;

    QDBusPendingCall asyncCall = bus.asyncCall(message);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, token, interactive](QDBusPendingCallWatcher* callWatcher) {
        callWatcher->deleteLater();
        QDBusPendingReply<QDBusObjectPath> reply = *callWatcher;

        if (reply.isError()) {
            qWarning() << "[WaylandCapture] Portal Screenshot error:" << reply.error().message();
            // Fallback to direct capture if portal method failed
            performDirectCapture();
            return;
        }

        QString requestPath = reply.value().path();

        // Connect to Response signal on requestPath
        QDBusConnection::sessionBus().connect(
            PORTAL_SERVICE,
            requestPath,
            REQUEST_INTERFACE,
            "Response",
            this,
            SLOT(onPortalResponse(uint, QVariantMap))
        );
    });
}

void WaylandCaptureManager::pickColor() {
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        emit captureFailed(tr("D-Bus connection is not available"));
        return;
    }

    QString token = QString("sshot_col_%1_%2").arg(QDateTime::currentMSecsSinceEpoch())
                                             .arg(QRandomGenerator::global()->generate());

    QVariantMap options;
    options["handle_token"] = token;

    QDBusMessage message = QDBusMessage::createMethodCall(
        PORTAL_SERVICE, PORTAL_PATH, PORTAL_INTERFACE, "PickColor"
    );
    message << QString("") << options;

    QDBusPendingCall asyncCall = bus.asyncCall(message);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(asyncCall, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher* callWatcher) {
        callWatcher->deleteLater();
        QDBusPendingReply<QDBusObjectPath> reply = *callWatcher;

        if (reply.isError()) {
            qWarning() << "[WaylandCapture] Portal PickColor error:" << reply.error().message();
            emit captureFailed(reply.error().message());
            return;
        }

        QString requestPath = reply.value().path();
        QDBusConnection::sessionBus().connect(
            PORTAL_SERVICE,
            requestPath,
            REQUEST_INTERFACE,
            "Response",
            this,
            SLOT(onPortalColorResponse(uint, QVariantMap))
        );
    });
}

void WaylandCaptureManager::handlePortalResponse(uint response, const QVariantMap& results, bool isColorPicker) {
    if (response != 0) {
        emit captureCancelled();
        return;
    }

    if (isColorPicker) {
        if (results.contains("color")) {
            QVariant colVar = results.value("color");
            // Portal color is a struct or sequence of 3 doubles (r, g, b in [0.0, 1.0])
            double r = 0.0, g = 0.0, b = 0.0;
            if (colVar.canConvert<QDBusArgument>()) {
                const QDBusArgument& arg = colVar.value<QDBusArgument>();
                arg.beginStructure();
                arg >> r >> g >> b;
                arg.endStructure();
            } else if (colVar.typeId() == QMetaType::QVariantList) {
                QVariantList list = colVar.toList();
                if (list.size() >= 3) {
                    r = list[0].toDouble();
                    g = list[1].toDouble();
                    b = list[2].toDouble();
                }
            }

            QColor color = QColor::fromRgbF(qBound(0.0, r, 1.0),
                                            qBound(0.0, g, 1.0),
                                            qBound(0.0, b, 1.0));
            emit colorPicked(color, color.name(QColor::HexRgb).toUpper());
        }
        return;
    }

    if (results.contains("uri")) {
        QString uriStr = results.value("uri").toString();
        QUrl url(uriStr);
        QString localPath = url.isLocalFile() ? url.toLocalFile() : uriStr;

        QImage img(localPath);
        if (!img.isNull()) {
            emit screenshotReady(QPixmap::fromImage(img));
        } else {
            emit captureFailed(tr("Failed to read image from portal path: %1").arg(localPath));
        }
    }
}

void WaylandCaptureManager::onPortalResponse(uint response, const QVariantMap& results) {
    handlePortalResponse(response, results, false);
}

void WaylandCaptureManager::onPortalColorResponse(uint response, const QVariantMap& results) {
    handlePortalResponse(response, results, true);
}

void WaylandCaptureManager::performDirectCapture() {
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QPixmap pix = screen->grabWindow(0);
        if (!pix.isNull()) {
            emit screenshotReady(pix);
            return;
        }
    }
    emit captureFailed(tr("Screen capture failed"));
}

#include "moc_WaylandCaptureManager.cpp"
