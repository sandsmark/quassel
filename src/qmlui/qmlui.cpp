#include "qmlui.h"

#include "buffermodel.h"
#include "types.h"
#include "util.h"
#include "../../version.h"

#ifdef USE_UBUNTU
#include "mainwin.h"
#endif

#include "chatlinemodel.h"
#include "qmluiaccountmodel.h"
#include "qmluibuffermodel.h"
#include "qmluiapplication.h"
#include "qmluimessagemodel.h"
#include "qmluimessageprocessor.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>

QmlUi::QmlUi() : AbstractUi()
{
    Q_INIT_RESOURCE(qml);

    Quassel::loadTranslation(QLocale::system());
}


QmlUi::~QmlUi()
{
}


void QmlUi::init()
{
#ifdef USE_UBUNTU
    MainWin *mainWin = new MainWin();
    QQmlEngine *engine = mainWin->engine();
#else
    QQmlApplicationEngine *engine = new QQmlApplicationEngine;
#endif

    qmlRegisterType<QmlUiAccountModel>("Quassel", 0, 1, "AccountModel");
    qmlRegisterType<QmlUiBufferModel>("Quassel", 0, 1, "BufferModel");
    qmlRegisterType<QmlUiMessageModel>("Quassel", 0, 1, "MessageModel");
    engine->rootContext()->setContextProperty("bugUrl", QUASSEL_BUG_URL);
    engine->rootContext()->setContextProperty("CoreConnection", Client::coreConnection());


#ifdef USE_UBUNTU
    mainWin->setSource(QUrl(QStringLiteral("qrc:///qml/MainView.qml")));
#else
    engine->load(QUrl(QStringLiteral("qrc:///qml/MainView.qml")));
#endif
}


MessageModel *QmlUi::createMessageModel(QObject *parent)
{
    return new ChatLineModel(parent);
}


AbstractMessageProcessor *QmlUi::createMessageProcessor(QObject *parent)
{
    return new QmlUiMessageProcessor(parent);
}

QmlUi *QmlUi::instance()
{
    static QmlUi qmlUiInstance;
    return &qmlUiInstance;
}


void QmlUi::connectedToCore()
{
    emit coreConnected();
}


void QmlUi::disconnectedFromCore()
{
    emit coreDisconnected();
    AbstractUi::disconnectedFromCore();
}
