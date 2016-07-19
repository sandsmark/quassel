#include "qmluiapplication.h"

#include "qmlui.h"
#include "client.h"
#include <QCoreApplication>

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
        Client::init(QmlUi::instance());
        QmlUi::instance()->init();
        return true;
    }
    return false;
}

QmlUiApplication::~QmlUiApplication()
{
    Client::destroy();
}

void QmlUiApplication::quit()
{
    qApp->quit();
}
