#ifndef CHATLINEMODELITEM_H_
#define CHATLINEMODELITEM_H_

#include "messagemodel.h"

class ChatLineModelItem : public MessageModelItem
{
public:
    ChatLineModelItem(const Message &);

    virtual QVariant data(int column, int role) const;
    virtual bool setData(int column, const QVariant &value, int role);

    virtual inline const Message &message() const { return _styledMsg; }
    virtual inline const QDateTime &timestamp() const { return _styledMsg.timestamp(); }
    virtual inline const MsgId &msgId() const { return _styledMsg.msgId(); }
    virtual inline const BufferId &bufferId() const { return _styledMsg.bufferId(); }
    virtual inline void setBufferId(BufferId bufferId) { _styledMsg.setBufferId(bufferId); }
    virtual inline Message::Type msgType() const { return _styledMsg.type(); }
    virtual inline Message::Flags msgFlags() const { return _styledMsg.flags(); }

private:
    Message _styledMsg;
};


#endif
