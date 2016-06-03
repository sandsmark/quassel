#include "chatlinemodelitem.h"
#include "chatlinemodel.h"
#include "qmlui.h"


ChatLineModelItem::ChatLineModelItem(const Message &msg)
    : MessageModelItem(),
    _styledMsg(msg)
{
    if (!msg.sender().contains('!'))
        _styledMsg.setFlags(msg.flags() |= Message::ServerMsg);
}


bool ChatLineModelItem::setData(int column, const QVariant &value, int role)
{
    switch (role) {
    case MessageModel::FlagsRole:
        _styledMsg.setFlags((Message::Flags)value.toUInt());
        return true;
    default:
        return MessageModelItem::setData(column, value, role);
    }
}


QVariant ChatLineModelItem::data(int column, int role) const
{
    return MessageModelItem::data(column, role);
}
