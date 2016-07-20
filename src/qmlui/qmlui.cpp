#include "qmlui.h"

#include "clientbufferviewmanager.h"

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

QmlUi::QmlUi() : AbstractUi(),
    m_bufferModel(nullptr)
{
    Q_INIT_RESOURCE(qml);

    Quassel::loadTranslation(QLocale::system());
#ifdef USE_UBUNTU
    _mainWin = nullptr;
#endif
}


QmlUi::~QmlUi()
{
#ifdef USE_UBUNTU
    delete _mainWin;
#endif
}


void QmlUi::init()
{
#ifdef USE_UBUNTU
    if (!_mainWin) {
        _mainWin = new MainWin();
    }
    QQmlEngine *engine = _mainWin->engine();
#else
    QQmlApplicationEngine *engine = new QQmlApplicationEngine(this);
#endif
    m_bufferModel = new QmlUiBufferModel(this);

    qmlRegisterType<QmlUiAccountModel>("Quassel", 0, 1, "AccountModel");
    qmlRegisterType<QmlUiMessageModel>("Quassel", 0, 1, "MessageModel");
    engine->rootContext()->setContextProperty("bugUrl", QUASSEL_BUG_URL);
    engine->rootContext()->setContextProperty("CoreConnection", Client::coreConnection());
    engine->rootContext()->setContextProperty("BacklogManager", Client::backlogManager());
    engine->rootContext()->setContextProperty("BufferModel", m_bufferModel);


#ifdef USE_UBUNTU
    _mainWin->setSource(QUrl(QStringLiteral("qrc:///qml/MainView.qml")));
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
    static QmlUi *qmlUiInstance = nullptr;
    if (!qmlUiInstance) {
        qmlUiInstance = new QmlUi;
    }
    return qmlUiInstance;
}


void QmlUi::connectedToCore()
{
    emit coreConnected();

    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigAdded(int)), this, SLOT(addBufferView(int)));
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigDeleted(int)), this, SLOT(removeBufferView(int)));
    connect(Client::bufferViewManager(), SIGNAL(initDone()), this, SLOT(loadLayout()));
}


void QmlUi::disconnectedFromCore()
{
    emit coreDisconnected();
    AbstractUi::disconnectedFromCore();
}

void QmlUi::addBufferView(int bufferViewConfigId)
{
    m_bufferModel->setConfig(Client::bufferViewManager()->clientBufferViewConfig(bufferViewConfigId));
}

void QmlUi::removeBufferView(int)
{

}

void QmlUi::loadLayout()
{

}
