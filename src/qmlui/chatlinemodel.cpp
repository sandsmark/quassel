#include "chatlinemodel.h"
#include "qmlui.h"

ChatLineModel::ChatLineModel(QObject *parent)
    : MessageModel(parent)
{
}


QVariant ChatLineModel::data(const QModelIndex & index, int role) const
{
    return messageItemAt(index.row())->data(index.column(), role);
}


void ChatLineModel::insertMessages__(int pos, const QList<Message> &messages)
{
    for (int i = 0; i < messages.count(); i++) {
        _messageList.insert(pos, ChatLineModelItem(messages[i]));
        pos++;
    }
}


Message ChatLineModel::takeMessageAt(int i)
{
    Message msg = _messageList[i].message();
    _messageList.removeAt(i);
    return msg;
}


void ChatLineModel::styleChanged()
{
    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
}
