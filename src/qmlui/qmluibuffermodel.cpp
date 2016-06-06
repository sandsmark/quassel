#include "qmluibuffermodel.h"

#include "client.h"

QmlUiBufferModel::QmlUiBufferModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSourceModel(Client::networkModel());
}


QVariant QmlUiBufferModel::data(const QModelIndex &index, int role) const
{
    QModelIndex sourceIndex(mapToSource(index));
    switch(role) {
    case NetworkModel::NetworkIdRole:
    {
        BufferId bufferId(sourceIndex.data(NetworkModel::BufferIdRole).value<BufferId>());
        return dynamic_cast<NetworkModel *>(sourceModel())->networkName(bufferId);
    }
    case NetworkModel::BufferIdRole:
    {
        NetworkId networkId(sourceIndex.data(NetworkModel::NetworkIdRole).value<NetworkId>());
        QVariantList bufferModel;
        QModelIndex child(sourceIndex.child(sourceIndex.row(), sourceIndex.column()));
        while (child.isValid()) {
            BufferId bufferId(child.data(role).value<BufferId>());
            QString bufferName(dynamic_cast<NetworkModel *>(sourceModel())->bufferName(bufferId));
            int activity(child.data(NetworkModel::BufferActivityRole).value<int>());
            QVariantMap map;
            map.insert(QStringLiteral("networkId"), networkId.toInt());
            map.insert(QStringLiteral("bufferId"), bufferId.toInt());
            map.insert(QStringLiteral("bufferName"), bufferName);
            map.insert(QStringLiteral("activity"), activity);
            bufferModel.append(map);
            child = child.sibling(child.row() + 1, child.column());
        }
        return bufferModel;
    }
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> QmlUiBufferModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NetworkModel::NetworkIdRole, "network");
    roles.insert(NetworkModel::BufferIdRole, "buffers");
    return roles;
}
