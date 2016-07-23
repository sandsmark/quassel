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
#include "bufferviewoverlay.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

QmlUi::QmlUi() : AbstractUi(),
    m_bufferModel(nullptr)
{
    Q_INIT_RESOURCE(qml);

#if defined(Q_OS_ANDROID)
    QQuickStyle::setStyle("Material");
#elif defined(Q_OS_WIN)
    QQuickStyle::setStyle("Universal");
#endif

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

    qmlRegisterType<QmlUiMessageModel>("Quassel", 0, 1, "MessageModel");
    qmlRegisterSingletonType<QmlUiAccountModel>("Quassel", 0, 1, "AccountModel", [](QQmlEngine *, QJSEngine*) -> QObject* {
        return new QmlUiAccountModel;
    });
    qmlRegisterSingletonType<CoreConnection>("Quassel", 0, 1, "CoreConnection", [](QQmlEngine *engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(Client::coreConnection(), QQmlEngine::CppOwnership);
        return Client::coreConnection();
    });
    qmlRegisterSingletonType<BacklogManager>("Quassel", 0, 1, "BacklogManager", [](QQmlEngine *engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(Client::backlogManager(), QQmlEngine::CppOwnership);
        return Client::backlogManager();
    });
    qmlRegisterSingletonType<BacklogManager>("Quassel", 0, 1, "Client", [](QQmlEngine *engine, QJSEngine*) -> QObject* {
        engine->setObjectOwnership(Client::instance(), QQmlEngine::CppOwnership);
        return Client::instance();
    });
    engine->rootContext()->setContextProperty("bugUrl", QUASSEL_BUG_URL);
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
    qDebug() << "Adding buffer view:" << bufferViewConfigId;
    Client::bufferViewOverlay()->addView(bufferViewConfigId);
    m_bufferModel->setConfig(Client::bufferViewManager()->clientBufferViewConfig(bufferViewConfigId));
}

void QmlUi::removeBufferView(int)
{

}

void QmlUi::loadLayout()
{

}
