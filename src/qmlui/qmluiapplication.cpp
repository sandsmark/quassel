#include "qmluiapplication.h"

#include "qmlui.h"
#include "mainwin.h"
#include "client.h"

QmlUiApplication::QmlUiApplication(int &argc, char **argv)
    : QGuiApplication(argc, argv),
    Quassel()
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    setDataDirPaths(findDataDirPaths());
    setRunMode(Quassel::ClientOnly);
    qInstallMessageHandler(Client::logMessage);
}

bool QmlUiApplication::init()
{
    if (Quassel::init()) {
        QmlUi *gui = new QmlUi();
        Client::init(gui);
        gui->init();
        // If initialization goes wrong, the window won't show
        return QmlUi::mainWindow()->isVisible();
    }
    return false;
}

QmlUiApplication::~QmlUiApplication()
{
    Client::destroy();
}

void QmlUiApplication::quit()
{
    QmlUi::mainWindow()->quit();
}
