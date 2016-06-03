#include "qmlui.h"

#include "buffermodel.h"
#include "types.h"
#include "util.h"

#include "chatlinemodel.h"
#include "mainwin.h"
#include "qmluimessageprocessor.h"

QmlUi *QmlUi::_instance = 0;
MainWin *QmlUi::_mainWin = 0;

QmlUi::QmlUi() : AbstractUi()
{
    if (_instance != 0) {
        qWarning() << "QmlUi has been instantiated again!";
        return;
    }
    _instance = this;

    Quassel::loadTranslation(QLocale::system());

    _mainWin = new MainWin();

    connect(_mainWin, SIGNAL(connectToCore(const QVariantMap &)), this, SIGNAL(connectToCore(const QVariantMap &)));
    connect(_mainWin, SIGNAL(disconnectFromCore()), this, SIGNAL(disconnectFromCore()));
}


QmlUi::~QmlUi()
{
    delete _mainWin;
    _mainWin = 0;
    _instance = 0;
}


void QmlUi::init()
{
    _mainWin->init();
}


MessageModel *QmlUi::createMessageModel(QObject *parent)
{
    return new ChatLineModel(parent);
}


AbstractMessageProcessor *QmlUi::createMessageProcessor(QObject *parent)
{
    return new QmlUiMessageProcessor(parent);
}


void QmlUi::connectedToCore()
{
    _mainWin->connectedToCore();
}


void QmlUi::disconnectedFromCore()
{
    _mainWin->disconnectedFromCore();
    AbstractUi::disconnectedFromCore();
}
