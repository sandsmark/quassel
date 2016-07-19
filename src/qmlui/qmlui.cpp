#include "qmlui.h"

#include "buffermodel.h"
#include "types.h"
#include "util.h"
#include "../../version.h"

#include "chatlinemodel.h"
#include "mainwin.h"
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
    delete _mainWin;
    _mainWin = 0;
}


void QmlUi::init()
{
    if (!_mainWin) {
        _mainWin = new MainWin();
    }
    QQmlEngine *engine = _mainWin->engine();

    qmlRegisterType<QmlUiAccountModel>("Quassel", 0, 1, "AccountModel");
    qmlRegisterType<QmlUiBufferModel>("Quassel", 0, 1, "BufferModel");
    qmlRegisterType<QmlUiMessageModel>("Quassel", 0, 1, "MessageModel");
    engine->rootContext()->setContextProperty("bugUrl", QUASSEL_BUG_URL);

    _mainWin->setSource(QUrl(QStringLiteral("qrc:///qml/MainView.qml")));
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
