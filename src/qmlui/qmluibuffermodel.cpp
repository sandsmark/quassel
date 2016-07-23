#include "qmluibuffermodel.h"

#include "bufferviewconfig.h"
#include "clientbufferviewconfig.h"

#include "client.h"

QmlUiBufferModel::QmlUiBufferModel(QObject *parent)
    : QSortFilterProxyModel(parent),
      m_config(nullptr)
{
    connect(Client::instance(), SIGNAL(coreConnectionStateChanged(bool)), this, SIGNAL(connectedChanged()));
    setDynamicSortFilter(true);
    setSourceModel(Client::networkModel());
    setSortRole(Qt::DisplayRole);
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
        QModelIndex ourChild(index.child(0, sourceIndex.column()));
        while (ourChild.isValid()) {
            QModelIndex child(mapToSource(ourChild));
            BufferId bufferId(child.data(role).value<BufferId>());
            QString bufferName(dynamic_cast<NetworkModel *>(sourceModel())->bufferName(bufferId));
            int activity(child.data(NetworkModel::BufferActivityRole).value<int>());
            QVariantMap map;
            map.insert(QStringLiteral("networkId"), networkId.toInt());
            map.insert(QStringLiteral("bufferId"), bufferId.toInt());
            map.insert(QStringLiteral("bufferName"), bufferName);
            map.insert(QStringLiteral("activity"), activity);
            bufferModel.append(map);
            ourChild = ourChild.sibling(ourChild.row() + 1, ourChild.column());
        }
        return bufferModel;
    }
    default:
        return sourceModel()->data(sourceIndex, role);
    }
}


QHash<int, QByteArray> QmlUiBufferModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NetworkModel::NetworkIdRole, "network");
    roles.insert(NetworkModel::BufferIdRole, "buffers");
    roles.unite(QSortFilterProxyModel::roleNames());
    return roles;
}

void QmlUiBufferModel::setConfig(ClientBufferViewConfig *config)
{
    m_config = config;
    connect(m_config, SIGNAL(initDone()), this, SLOT(invalidate()));
}

int QmlUiBufferModel::getBufferId(const QModelIndex &index)
{
    QModelIndex sourceIndex(mapToSource(index));
    return BufferId(sourceIndex.data(NetworkModel::BufferIdRole).value<BufferId>()).toInt();
}

bool QmlUiBufferModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int leftItemType = sourceModel()->data(source_left, NetworkModel::ItemTypeRole).toInt();
    int rightItemType = sourceModel()->data(source_right, NetworkModel::ItemTypeRole).toInt();
    int itemType = leftItemType & rightItemType;
    switch (itemType) {
    case NetworkModel::BufferItemType:
        return bufferLessThan(source_left, source_right);
    default:
        return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}

bool QmlUiBufferModel::bufferLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    BufferId leftBufferId = sourceModel()->data(source_left, NetworkModel::BufferIdRole).value<BufferId>();
    BufferId rightBufferId = sourceModel()->data(source_right, NetworkModel::BufferIdRole).value<BufferId>();
    if (m_config) {
        int leftPos = m_config->bufferList().indexOf(leftBufferId);
        int rightPos = m_config->bufferList().indexOf(rightBufferId);
        if (leftPos == -1 && rightPos == -1)
            return QSortFilterProxyModel::lessThan(source_left, source_right);
        if (leftPos == -1 || rightPos == -1)
            return !(leftPos < rightPos);
        return leftPos < rightPos;
    }
    else
        return bufferIdLessThan(leftBufferId, rightBufferId);
}

bool QmlUiBufferModel::bufferIdLessThan(const BufferId &left, const BufferId &right) const
{
    Q_CHECK_PTR(Client::networkModel());
    if (!Client::networkModel())
        return true;

    QModelIndex leftIndex = Client::networkModel()->bufferIndex(left);
    QModelIndex rightIndex = Client::networkModel()->bufferIndex(right);

    int leftType = Client::networkModel()->data(leftIndex, NetworkModel::BufferTypeRole).toInt();
    int rightType = Client::networkModel()->data(rightIndex, NetworkModel::BufferTypeRole).toInt();

    if (leftType != rightType)
        return leftType < rightType;
    else
        return QString::compare(Client::networkModel()->data(leftIndex, Qt::DisplayRole).toString(), Client::networkModel()->data(rightIndex, Qt::DisplayRole).toString(), Qt::CaseInsensitive) < 0;
}

bool QmlUiBufferModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex child = sourceModel()->index(source_row, 0, source_parent);

    NetworkModel::ItemType childType = (NetworkModel::ItemType)sourceModel()->data(child, NetworkModel::ItemTypeRole).toInt();
    if (childType == NetworkModel::NetworkItemType) {
        return true;
    }
    if (childType != NetworkModel::BufferItemType) {
        return false;
    }

    if (!m_config) {
        return true;
    }
    BufferId bufferId = sourceModel()->data(child, NetworkModel::BufferIdRole).value<BufferId>();

    int activityLevel = sourceModel()->data(child, NetworkModel::BufferActivityRole).toInt();
    if (!m_config->bufferList().contains(bufferId)) {
        // add the buffer if...
        if (m_config->isInitialized()
            && !m_config->removedBuffers().contains(bufferId) // it hasn't been manually removed and either
            && ((m_config->addNewBuffersAutomatically() && !m_config->temporarilyRemovedBuffers().contains(bufferId)) // is totally unknown to us (a new buffer)...
                || (m_config->temporarilyRemovedBuffers().contains(bufferId) && activityLevel > BufferInfo::OtherActivity))) { // or was just temporarily hidden and has a new message waiting for us.

            // Add buffer
            int pos = m_config->bufferList().count();
            bool lt;
            for (int i = 0; i < m_config->bufferList().count(); i++) {
                if (m_config->sortAlphabetically())
                    lt = bufferIdLessThan(bufferId, m_config->bufferList()[i]);
                else
                    lt = bufferId < m_config->bufferList()[i];

                if (lt) {
                    pos = i;
                    break;
                }
            }
            m_config->requestAddBuffer(bufferId, pos);
        }
        // note: adding the buffer to the valid list does not temper with the following filters ("show only channels" and stuff)
        return false;
    }

    return true;
}


bool QmlUiBufferModel::connected() const
{
    return Client::isConnected();
}
