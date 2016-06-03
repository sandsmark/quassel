#include "qmluimessagemodel.h"

#include "client.h"

QmlUiMessageModel::QmlUiMessageModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // _showMeta = true;
    setDynamicSortFilter(true);
    setFilterRole(MessageModel::BufferIdRole);
    setSourceModel(Client::messageModel());
}


QVariant QmlUiMessageModel::data(const QModelIndex &index, int role) const
{
    QModelIndex sourceIndex(mapToSource(index));
    Message message(sourceModel()->data(sourceIndex, MessageModel::MessageRole).value<Message>());
    switch(role) {
    case MessageModel::UserRole:
        return message.sender();
    case MessageModel::MessageRole:
        return message.contents();
    case MessageModel::TimestampRole:
        return message.timestamp();
    case MessageModel::EditRole:
    {
        // Does the next row share the same sender? Should we have a divider?
        QModelIndex next(index.sibling(index.row() + 1, index.column()));
        if (!next.isValid())
            return true;
        return next.data(MessageModel::UserRole) != index.data(MessageModel::UserRole);
    }
    case MessageModel::MsgIdRole:
        return QVariant::fromValue<int>(message.msgId().toInt());
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> QmlUiMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(MessageModel::UserRole, "sender");
    roles.insert(MessageModel::MessageRole, "message");
    roles.insert(MessageModel::TimestampRole, "timestamp");
    roles.insert(MessageModel::EditRole, "divider");
    roles.insert(MessageModel::MsgIdRole, "msgId");
    return roles;
}


bool QmlUiMessageModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex(sourceModel()->index(sourceRow, 0, sourceParent));
    Message message(sourceIndex.data(MessageModel::MessageRole).value<Message>());
    if (message.type() > Message::Action && !_showMeta)
        return false;
    return message.bufferId().toInt() == _bufferId;
}
