#ifndef QMLUIMESSAGEMODEL_H_
#define QMLUIMESSAGEMODEL_H_

#include "messagemodel.h"
#include "client.h"
#include "networkmodel.h"

#include <QSortFilterProxyModel>

class QmlUiMessageModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int bufferId READ bufferId WRITE setBufferId NOTIFY bufferIdChanged)
    Q_PROPERTY(QString bufferName READ bufferName NOTIFY bufferNameChanged)
    Q_PROPERTY(QString topic READ topic NOTIFY topicChanged)
    Q_PROPERTY(int lastSeenMsgId READ lastSeenMsgId NOTIFY lastSeenMsgIdChanged)
    // For convenience: per network, not per buffer
    Q_PROPERTY(int networkId READ networkId WRITE setNetworkId NOTIFY networkIdChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString nick READ nick NOTIFY nickChanged)

public:
    QmlUiMessageModel(QObject *parent = 0);

    enum {
        StyledMessageRole,
        PreviousSiblingRole,
        NextSiblingRole,
        HighlightRole,
        SelfRole,
        ActionRole,
    };

    // QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    // QSortFilterProxyModel
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public Q_SLOTS:
    Q_INVOKABLE void userInput(const QString& message);

Q_SIGNALS:
    void bufferIdChanged();
    void bufferNameChanged();
    void lastSeenMsgIdChanged();
    void topicChanged();
    void networkIdChanged();
    void connectedChanged();
    void nickChanged();

protected:

protected slots:

private:
    NetworkId _networkId;
    const Network *_network;
    inline void setNetworkId(int networkId) {
        if (_network) {
            disconnect(_network, SIGNAL(connectedSet(bool)), this, SIGNAL(connectedChanged()));
            disconnect(_network, SIGNAL(myNickSet(const QString &)), this, SLOT(nickChanged()));
        }
        _networkId = NetworkId(networkId);
        _network = Client::network(_networkId);
        if (_network) {
            connect(_network, SIGNAL(connectedSet(bool)), this, SIGNAL(connectedChanged()));
            connect(_network, SIGNAL(myNickSet(const QString &)), this, SIGNAL(nickChanged()));
        }
        Q_EMIT networkIdChanged();
        Q_EMIT connectedChanged();
        Q_EMIT nickChanged();
    }
    inline int networkId() const { return _networkId.toInt(); }

    inline QVariant networkData(int role) const {
        NetworkModel* model(Client::networkModel());
        return model->bufferIndex(_bufferId).data(role);
    }

    int _bufferId;
    inline int bufferId() const { return _bufferId; }
    inline void setBufferId(int bufferId) {
        _bufferId = bufferId;
        invalidateFilter();
        Q_EMIT bufferIdChanged();
    }

    inline QString bufferName() const { return Client::networkModel()->bufferName(_bufferId); }
    inline QString topic() const {
        IrcChannel* channel(networkData(NetworkModel::IrcChannelRole).value<IrcChannel *>());
        return channel ? channel->topic() : QString("");
    }
    inline int lastSeenMsgId() const { return networkData(NetworkModel::MarkerLineMsgIdRole).toInt(); }
    inline bool connected() const {
        return _network ? _network->isConnected() : false;
    }
    inline QString nick() const {
        return _network ? _network->myNick() : QString("");
    }
    // TODO: Expose property
    bool _showMeta;
};

#endif
