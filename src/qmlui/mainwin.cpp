#include "mainwin.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>

#include "buffermodel.h"
#include "bufferviewoverlay.h"
#include "clientignorelistmanager.h"
#include "clientbacklogmanager.h"
#include "clienttransfermanager.h"
#include "clientbufferviewmanager.h"
#include "coreconnection.h"

#include "qmluibuffermodel.h"
#include "qmluiapplication.h"
#include "qmluimessagemodel.h"
#include "qmluimessageprocessor.h"


MainWin::MainWin(QWindow *parent)
    : QQuickView(parent),
    _layoutLoaded(false),
    _activeBufferViewIndex(-1)
{
    Q_INIT_RESOURCE(qml);

    // FIXME: setAttribute(Qt::WA_DeleteOnClose, false); // we delete the mainwin manually

    // TODO: QApplication::setQuitOnLastWindowClosed(false);

    updateIcon();
}


void MainWin::init()
{
    connect(Client::instance(), SIGNAL(networkCreated(NetworkId)), SLOT(clientNetworkCreated(NetworkId)));
    connect(Client::instance(), SIGNAL(networkRemoved(NetworkId)), SLOT(clientNetworkRemoved(NetworkId)));
    connect(Client::messageModel(), SIGNAL(rowsInserted(const QModelIndex &, int, int)),
        SLOT(messagesInserted(const QModelIndex &, int, int)));

    // TODO: connect(Client::coreConnection(), SIGNAL(startCoreSetup(QVariantList)), SLOT(showCoreConfigWizard(QVariantList)));
    connect(Client::coreConnection(), SIGNAL(connectionErrorPopup(QString)), SLOT(handleCoreConnectionError(QString)));
    connect(Client::coreConnection(), SIGNAL(userAuthenticationRequired(CoreAccount *, bool *, QString)), SLOT(userAuthenticationRequired(CoreAccount *, bool *, QString)));
    connect(Client::coreConnection(), SIGNAL(handleNoSslInClient(bool *)), SLOT(handleNoSslInClient(bool *)));
    connect(Client::coreConnection(), SIGNAL(handleNoSslInCore(bool *)), SLOT(handleNoSslInCore(bool *)));
#ifdef HAVE_SSL
    connect(Client::coreConnection(), SIGNAL(handleSslErrors(const QSslSocket *, bool *, bool *)), SLOT(handleSslErrors(const QSslSocket *, bool *, bool *)));
#endif

    // TODO: connect(bufferWidget(), SIGNAL(currentChanged(BufferId)), SLOT(currentBufferChanged(BufferId)));
    // -> Client::instance()->markBufferAsRead(buffer);

    setDisconnectedState(); // Disable menus and stuff

    qmlRegisterType<QmlUiBufferModel>("Quassel", 0, 1, "BufferModel");
    qmlRegisterType<QmlUiMessageModel>("Quassel", 0, 1, "MessageModel");

    setSource(QUrl(QStringLiteral("qrc:///qml/MainView.qml")));
    if (errors().count() > 0) {
        return;
    }
    setResizeMode(QQuickView::SizeRootObjectToView);
    showNormal();

    // restore mainwin state
    /* TODO: UiSettings s;
    restoreStateFromSettings(s);*/

    CoreConnection *conn = Client::coreConnection();
    if (!conn->connectToCore()) {
        // No autoconnect selected (or no accounts)
        // FIXME: showCoreConnectionDlg();
    }
}


MainWin::~MainWin()
{
}


void MainWin::quit()
{
    /* TODO: UiSettings s;
    saveStateToSettings(s); */
    saveLayout();
    QCoreApplication::quit();
}


/*void MainWin::saveStateToSettings(UiSettings &s)
{
    BufferId lastBufId = Client::bufferModel()->currentBuffer();
    if (lastBufId.isValid())
        s.setValue("LastUsedBufferId", lastBufId.toInt());
}


void MainWin::restoreStateFromSettings(UiSettings &s)
{
    Q_UNUSED(s)
    // FIXME: Server mode?
    if ((Quassel::isOptionSet("hidewindow")))
        QmlUi::hideMainWidget();
    else
        show();
}*/


void MainWin::updateIcon()
{
    QIcon icon;
    if (Client::isConnected())
        icon = QIcon::fromTheme("quassel", QIcon(":/icons/quassel-128.png"));
    else
        icon = QIcon::fromTheme("quassel-inactive", QIcon(":/icons/quassel-128.png"));
    setIcon(icon);
}


void MainWin::addBufferView(int bufferViewConfigId)
{
    addBufferView(Client::bufferViewManager()->clientBufferViewConfig(bufferViewConfigId));
}


void MainWin::addBufferView(ClientBufferViewConfig *config)
{
    if (!config)
        return;

    if (!activeBufferView())
        nextBufferView();
}

void MainWin::nextBufferView()
{
    // TODO
}


void MainWin::connectedToCore()
{
    Q_CHECK_PTR(Client::bufferViewManager());
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigAdded(int)), this, SLOT(addBufferView(int)));
    connect(Client::bufferViewManager(), SIGNAL(bufferViewConfigDeleted(int)), this, SLOT(removeBufferView(int)));
    connect(Client::bufferViewManager(), SIGNAL(initDone()), this, SLOT(loadLayout()));

    // TODO: connect(Client::transferManager(), SIGNAL(transferAdded(const ClientTransfer*)), SLOT(showNewTransferDlg(const ClientTransfer*)));

    setConnectedState();
}

void MainWin::removeBufferView(int bufferViewConfigId)
{
    Q_UNUSED(bufferViewConfigId)
    // TODO
}

void MainWin::changeActiveBufferView(int bufferViewId)
{
    Q_UNUSED(bufferViewId)
    // TODO
}


void MainWin::previousBufferView()
{
    // TODO
}


void MainWin::setConnectedState()
{
    /* TODO:
    coll->action("ConnectCore")->setEnabled(false);
    coll->action("DisconnectCore")->setEnabled(true);
    coll->action("ChangePassword")->setEnabled(true);
    coll->action("CoreInfo")->setEnabled(true);

    if (isRemoteCoreOnly(action))
        action->setVisible(!Client::internalCore());
    disconnect(Client::backlogManager(), SIGNAL(updateProgress(int, int)), _msgProcessorStatusWidget, SLOT(setProgress(int, int)));
    */

    disconnect(Client::backlogManager(), SIGNAL(messagesRequested(const QString &)), this, SLOT(showStatusBarMessage(const QString &)));
    disconnect(Client::backlogManager(), SIGNAL(messagesProcessed(const QString &)), this, SLOT(showStatusBarMessage(const QString &)));
    if (!Client::internalCore()) {
        // TODO: connect(Client::backlogManager(), SIGNAL(updateProgress(int, int)), _msgProcessorStatusWidget, SLOT(setProgress(int, int)));
        connect(Client::backlogManager(), SIGNAL(messagesRequested(const QString &)), this, SLOT(showStatusBarMessage(const QString &)));
        connect(Client::backlogManager(), SIGNAL(messagesProcessed(const QString &)), this, SLOT(showStatusBarMessage(const QString &)));
    }

    showStatusBarMessage(tr("Connected to core."));
    updateIcon();

    if (Client::networkIds().isEmpty()) {
        // TODO: Show wizard
    }
    else {
        // Monolithic always preselects last used buffer - Client only if the connection died
        if (Client::coreConnection()->wasReconnect() || Quassel::runMode() == Quassel::Monolithic) {
            /* TODO: UiSettings s;
            BufferId lastUsedBufferId(s.value("LastUsedBufferId").toInt());
            if (lastUsedBufferId.isValid())
                Client::bufferModel()->switchToBuffer(lastUsedBufferId);*/
        }
    }
}


void MainWin::loadLayout()
{
    /* TODO: UiSettings s;
    int accountId = Client::currentCoreAccount().accountId().toInt();
    int bufferViewId = s.value(QString("ActiveBufferView-%1").arg(accountId), -1).toInt();
    if (bufferViewId >= 0)
        changeActiveBufferView(bufferViewId);*/

    _layoutLoaded = true;
}


BufferView* MainWin::activeBufferView() const
{
    return 0;
}


void MainWin::saveLayout()
{
}


void MainWin::disconnectedFromCore()
{
    // save core specific layout and remove bufferviews;
    saveLayout();
    _layoutLoaded = false;

    /* TODO: store last active buffer
    UiSettings s;
    BufferId lastBufId currentBuffer();
    if (lastBufId.isValid()) {
        s.setValue("LastUsedBufferId", lastBufId.toInt());
        // clear the current selection
        Client::bufferModel()->standardSelectionModel()->clearSelection();
    } */
    setDisconnectedState();
}


void MainWin::setDisconnectedState()
{
    showStatusBarMessage(tr("Not connected to core."));
    updateIcon();
}


void MainWin::userAuthenticationRequired(CoreAccount *account, bool *valid, const QString &errorMessage)
{
    Q_UNUSED(account)
    Q_UNUSED(valid)
    Q_UNUSED(errorMessage)
    // TODO: *valid = (dlg.exec() == QDialog::Accepted);
}


void MainWin::handleNoSslInClient(bool *accepted)
{
    Q_UNUSED(accepted)
    /* TODO:
    QMessageBox box(QMessageBox::Warning, tr("Unencrypted Connection"), tr("<b>Your client does not support SSL encryption</b>"),
        QMessageBox::Ignore|QMessageBox::Cancel, this);
    box.setInformativeText(tr("Sensitive data, like passwords, will be transmitted unencrypted to your Quassel core."));
    box.setDefaultButton(QMessageBox::Ignore);
    *accepted = box.exec() == QMessageBox::Ignore;
    */
}


void MainWin::handleNoSslInCore(bool *accepted)
{
    Q_UNUSED(accepted)
    /* TODO:
    QMessageBox box(QMessageBox::Warning, tr("Unencrypted Connection"), tr("<b>Your core does not support SSL encryption</b>"),
        QMessageBox::Ignore|QMessageBox::Cancel, this);
    box.setInformativeText(tr("Sensitive data, like passwords, will be transmitted unencrypted to your Quassel core."));
    box.setDefaultButton(QMessageBox::Ignore);
    *accepted = box.exec() == QMessageBox::Ignore;
    */
}


#ifdef HAVE_SSL

void MainWin::handleSslErrors(const QSslSocket *socket, bool *accepted, bool *permanently)
{
    Q_UNUSED(socket)
    Q_UNUSED(accepted)
    Q_UNUSED(permanently)
    /* TODO:
    QString errorString = "<ul>";
    foreach(const QSslError error, socket->sslErrors())
    errorString += QString("<li>%1</li>").arg(error.errorString());
    errorString += "</ul>";

    QMessageBox box(QMessageBox::Warning,
        tr("Untrusted Security Certificate"),
        tr("<b>The SSL certificate provided by the core at %1 is untrusted for the following reasons:</b>").arg(socket->peerName()),
        QMessageBox::Cancel, this);
    box.setInformativeText(errorString);
    box.addButton(tr("Continue"), QMessageBox::AcceptRole);
    box.setDefaultButton(box.addButton(tr("Show Certificate"), QMessageBox::HelpRole));

    QMessageBox::ButtonRole role;
    do {
        box.exec();
        role = box.buttonRole(box.clickedButton());
        if (role == QMessageBox::HelpRole) {
            SslInfoDlg dlg(socket, this);
            dlg.exec();
        }
    }
    while (role == QMessageBox::HelpRole);

    *accepted = role == QMessageBox::AcceptRole;
    if (*accepted) {
        QMessageBox box2(QMessageBox::Warning,
            tr("Untrusted Security Certificate"),
            tr("Would you like to accept this certificate forever without being prompted?"),
            0, this);
        box2.setDefaultButton(box2.addButton(tr("Current Session Only"), QMessageBox::NoRole));
        box2.addButton(tr("Forever"), QMessageBox::YesRole);
        box2.exec();
        *permanently =  box2.buttonRole(box2.clickedButton()) == QMessageBox::YesRole;
    }
    */
}


#endif /* HAVE_SSL */

void MainWin::handleCoreConnectionError(const QString &error)
{
    qDebug() << tr("Core Connection Error") << error;
}


/* TODO:
void MainWin::showCoreConnectionDlg()
{
    AccountId accId = dlg.selectedAccount();
    if (accId.isValid())
        Client::coreConnection()->connectToCore(accId);
}
*/


/********************************************************************************************************/

bool MainWin::event(QEvent *event)
{
    switch(event->type()) {
    case QEvent::WindowActivate: {
        BufferId bufferId = Client::bufferModel()->currentBuffer();
        if (bufferId.isValid())
            Client::instance()->markBufferAsRead(bufferId);
        break;
    }
    case QEvent::WindowDeactivate:
        /* TODO: if (bufferWidget()->autoMarkerLineOnLostFocus())
            bufferWidget()->setMarkerLine(); */
        break;
    default:
        break;
    }
    return QQuickView::event(event);
}


void MainWin::closeEvent(QCloseEvent *event)
{
    /* FIXME: Server mode?
    QmlUiApplication *app = qobject_cast<QmlUiApplication *> qApp;
    if (!app->closingDown() && s.value("MinimizeOnClose").toBool()) {
        QmlUi::hideMainWidget();
        event->ignore();
    }
    else */ {
        event->accept();
        quit();
    }
}


void MainWin::messagesInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent)

    bool hasFocus = isActive();

    for (int i = start; i <= end; i++) {
        QModelIndex idx = Client::messageModel()->index(i, MessageModel::ContentsColumn);
        if (!idx.isValid()) {
            qDebug() << "MainWin::messagesInserted(): Invalid model index!";
            continue;
        }
        Message::Flags flags = (Message::Flags)idx.data(MessageModel::FlagsRole).toInt();
        if (flags.testFlag(Message::Backlog) || flags.testFlag(Message::Self))
            continue;

        BufferId bufId = idx.data(MessageModel::BufferIdRole).value<BufferId>();
        BufferInfo::Type bufType = Client::networkModel()->bufferType(bufId);

        // check if bufferId belongs to the shown chatlists
        if (!(Client::bufferViewOverlay()->bufferIds().contains(bufId) ||
              Client::bufferViewOverlay()->tempRemovedBufferIds().contains(bufId)))
            continue;

        // check if it's the buffer currently displayed
        if (hasFocus && bufId == Client::bufferModel()->currentBuffer())
            continue;

        // only show notifications for higlights or queries
        if (bufType != BufferInfo::QueryBuffer && !(flags & Message::Highlight))
            continue;

        // and of course: don't notify for ignored messages
        if (Client::ignoreListManager() && Client::ignoreListManager()->match(idx.data(MessageModel::MessageRole).value<Message>(), Client::networkModel()->networkName(bufId)))
            continue;

        // seems like we have a legit notification candidate!
        /* TODO:
        QModelIndex senderIdx = Client::messageModel()->index(i, MessageModel::SenderColumn);
        QString sender = senderIdx.data(MessageModel::EditRole).toString();
        QString contents = idx.data(MessageModel::DisplayRole).toString();
        AbstractNotificationBackend::NotificationType type;

        if (bufType == BufferInfo::QueryBuffer && !hasFocus)
            type = AbstractNotificationBackend::PrivMsg;
        else if (bufType == BufferInfo::QueryBuffer && hasFocus)
            type = AbstractNotificationBackend::PrivMsgFocused;
        else if (flags & Message::Highlight && !hasFocus)
            type = AbstractNotificationBackend::Highlight;
        else
            type = AbstractNotificationBackend::HighlightFocused;

        QmlUi::instance()->invokeNotification(bufId, type, sender, contents);
        */
    }
}


void MainWin::clientNetworkCreated(NetworkId id)
{
    Q_UNUSED(id)
    /* TODO:
    const Network *net = Client::network(id);
    net->networkName()
    id.toInt()
    QVariant::fromValue<NetworkId>(id)
    connect(net, SIGNAL(updatedRemotely()), this, SLOT(clientNetworkUpdated()));
    insertAction(beforeAction, act); */
}


void MainWin::clientNetworkUpdated()
{
    const Network *net = qobject_cast<const Network *>(sender());
    if (!net)
        return;

    switch (net->connectionState()) {
    case Network::Initialized:
        // "network-connect"
        // if we have no currently selected buffer, jump to the first connecting statusbuffer
        if (false /*currentBuffer */) {
            QModelIndex idx = Client::networkModel()->networkIndex(net->networkId());
            if (idx.isValid()) {
                BufferId statusBufferId = idx.data(NetworkModel::BufferIdRole).value<BufferId>();
                Client::bufferModel()->switchToBuffer(statusBufferId);
            }
        }
        break;
    case Network::Disconnected:
        // "network-disconnect"
        break;
    default:
        // "network-wired"
        break;
    }
}


void MainWin::clientNetworkRemoved(NetworkId id)
{
    Q_UNUSED(id)
}


void MainWin::showStatusBarMessage(const QString &message)
{
    engine()->rootContext()->setContextProperty("statusbarMessage", message);
}
