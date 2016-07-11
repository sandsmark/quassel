#pragma once

#include <QQuickView>
#include <QWindow>
#include <QSslSocket>

#include "qmlui.h"

class BufferHotListFilter;
class BufferView;
class ClientBufferViewConfig;
class CoreAccount;
class BufferViewDock;
class BufferId;
class NetworkId;

//!\brief The main window of Quassel's QmlUi.
class MainWin
    : public QQuickView {
    Q_OBJECT

public:
    MainWin(QWindow *parent = 0);
    virtual ~MainWin();

    void init();

    void addBufferView(ClientBufferViewConfig *config);
    BufferView *activeBufferView() const;

    bool event(QEvent *event);

    static void flagRemoteCoreOnly(QObject *object) { object->setProperty("REMOTE_CORE_ONLY", true); }
    static bool isRemoteCoreOnly(QObject *object) { return object->property("REMOTE_CORE_ONLY").toBool(); }

    // void saveStateToSettings(UiSettings &);
    // void restoreStateFromSettings(UiSettings &);

public slots:
    void showStatusBarMessage(const QString &message);
    void nextBufferView();     //!< Activate the next bufferview
    void previousBufferView(); //!< Activate the previous bufferview

    //! Quit application
    void quit();

protected:
    void closeEvent(QCloseEvent *event);

protected slots:
    void connectedToCore();
    void setConnectedState();
    void disconnectedFromCore();
    void setDisconnectedState();

private slots:
    void addBufferView(int bufferViewConfigId);
    void removeBufferView(int bufferViewConfigId);

    void handleCoreConnectionError(const QString &errorMsg);
    void userAuthenticationRequired(CoreAccount *, bool *valid, const QString &errorMessage);
    void handleNoSslInClient(bool *accepted);
    void handleNoSslInCore(bool *accepted);
#ifdef HAVE_SSL
    void handleSslErrors(const QSslSocket *socket, bool *accepted, bool *permanently);
#endif

    void clientNetworkCreated(NetworkId);
    void clientNetworkRemoved(NetworkId);
    void clientNetworkUpdated();

    void loadLayout();
    void saveLayout();

    void changeActiveBufferView(int bufferViewId);

signals:
    void connectToCore(const QVariantMap &connInfo);
    void disconnectFromCore();

private:

    void setupActions();
    void setupHotList();

    void updateIcon();

    QList<BufferViewDock *> _bufferViews;

    bool _layoutLoaded;

    BufferHotListFilter *_bufferHotList;
    QHash<int, BufferId> _jumpKeyMap;
    int _activeBufferViewIndex;

    friend class QmlUi;
};
