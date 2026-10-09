#include <QApplication>
#include <QCommandLineParser>
#include <QLocalServer>
#include <QLocalSocket>
#include <QIcon>
#include <QClipboard>
#include <QDebug>
#include <QTimer>
#include <memory>
#include <malloc.h>

#include "core/SettingsManager.h"
#include "core/UpdateManager.h"
#include "core/IconManager.h"
#include "core/CrashHandler.h"
#include "tray/TrayManager.h"
#include "capture/WaylandCaptureManager.h"
#include "editor/MainWindow.h"

int main(int argc, char* argv[]) {
    // Prefer native Wayland platform plugin
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "wayland;xcb");
    }

    QApplication app(argc, argv);
    app.setApplicationName("s-shot-wayland");
    app.setApplicationDisplayName("S-Shot Wayland");
    app.setApplicationVersion("1.32");
    app.setOrganizationName("S-Shot");
    app.setWindowIcon(IconManager::getAppIcon());

    CrashHandler::init();

    QCommandLineParser parser;
    parser.setApplicationDescription("S-Shot Wayland: Lightweight Native Wayland Screenshot & Annotation Tool");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption trayOption("tray", "Start minimized in system tray.");
    parser.addOption(trayOption);

    QCommandLineOption fullscreenOption("fullscreen", "Capture fullscreen immediately.");
    parser.addOption(fullscreenOption);

    QCommandLineOption regionOption("region", "Capture selected region immediately.");
    parser.addOption(regionOption);

    QCommandLineOption editorOption("editor", "Open annotation editor.");
    parser.addOption(editorOption);

    QCommandLineOption colorpickerOption("colorpicker", "Pick color from screen immediately.");
    parser.addOption(colorpickerOption);

    QCommandLineOption settingsOption("settings", "Open settings dialog.");
    parser.addOption(settingsOption);

    parser.addPositionalArgument("file", "Image file to open", "[file]");
    parser.process(app);

    const QString serverName = "s-shot-wayland-single-instance-socket";
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (socket.waitForConnected(500)) {
        QStringList args = app.arguments();
        args.removeFirst();
        QByteArray data = args.join(";").toUtf8();
        socket.write(data);
        socket.waitForBytesWritten(1000);
        return 0;
    }

    QLocalServer server;
    QLocalServer::removeServer(serverName);
    server.listen(serverName);

    SettingsManager& settings = SettingsManager::instance();
    WaylandCaptureManager& captureMgr = WaylandCaptureManager::instance();

    std::unique_ptr<MainWindow> mainWindow;
    auto getMainWindow = [&mainWindow]() -> MainWindow* {
        if (!mainWindow) {
            mainWindow = std::make_unique<MainWindow>();
        }
        return mainWindow.get();
    };

    TrayManager trayManager(getMainWindow);
    trayManager.showTrayIcon();

    // Connect Wayland capture signals to editor
    QObject::connect(&captureMgr, &WaylandCaptureManager::screenshotReady, [&getMainWindow, &settings](const QPixmap& pix) {
        MainWindow* win = getMainWindow();
        win->addImageTab(pix);
        if (settings.openEditorAfterCapture()) {
            win->show();
            win->raise();
            win->activateWindow();
        }
    });

    QObject::connect(&captureMgr, &WaylandCaptureManager::colorPicked, [&trayManager](const QColor& color, const QString& hex) {
        QGuiApplication::clipboard()->setText(hex);
        trayManager.showMessage(
            QObject::tr("Colour Picked"),
            QObject::tr("Copied %1 to clipboard").arg(hex),
            QSystemTrayIcon::Information,
            3000
        );
    });

    // Handle single-instance messages from subsequent CLI invocations
    QObject::connect(&server, &QLocalServer::newConnection, [&server, &getMainWindow, &captureMgr, &trayManager]() {
        QLocalSocket* client = server.nextPendingConnection();
        if (!client) return;

        QObject::connect(client, &QLocalSocket::readyRead, [client, &getMainWindow, &captureMgr, &trayManager]() {
            QByteArray data = client->readAll();
            QString msg = QString::fromUtf8(data);
            QStringList args = msg.split(';', Qt::SkipEmptyParts);

            if (args.contains("--fullscreen")) {
                captureMgr.captureFullscreen();
            } else if (args.contains("--region")) {
                captureMgr.captureRegion();
            } else if (args.contains("--colorpicker")) {
                captureMgr.pickColor();
            } else if (args.contains("--editor")) {
                MainWindow* win = getMainWindow();
                win->show();
                win->raise();
                win->activateWindow();
            } else if (args.contains("--settings")) {
                MainWindow* win = getMainWindow();
                win->openSettingsDialog();
            } else {
                bool opened = false;
                for (const QString& arg : args) {
                    if (!arg.startsWith('-') && QFile::exists(arg)) {
                        MainWindow* win = getMainWindow();
                        win->openImage(arg);
                        win->show();
                        win->raise();
                        win->activateWindow();
                        opened = true;
                        break;
                    }
                }
                if (!opened) {
                    MainWindow* win = getMainWindow();
                    win->show();
                    win->raise();
                    win->activateWindow();
                }
            }
        });
    });

    // Check for updates on startup if enabled
    if (settings.autoCheckUpdates()) {
        QTimer::singleShot(3000, []() {
            UpdateManager::instance().checkForUpdates(true);
        });
    }

    // Process initial command line options
    if (parser.isSet(fullscreenOption)) {
        captureMgr.captureFullscreen();
    } else if (parser.isSet(regionOption)) {
        captureMgr.captureRegion();
    } else if (parser.isSet(colorpickerOption)) {
        captureMgr.pickColor();
    } else if (parser.isSet(settingsOption)) {
        MainWindow* win = getMainWindow();
        win->openSettingsDialog();
    } else if (!parser.positionalArguments().isEmpty()) {
        QString file = parser.positionalArguments().first();
        MainWindow* win = getMainWindow();
        win->openImage(file);
        win->show();
    } else if (!parser.isSet(trayOption) || parser.isSet(editorOption)) {
        MainWindow* win = getMainWindow();
        win->show();
    }

    return app.exec();
}
