#ifndef CHATLINEMODEL_H_
#define CHATLINEMODEL_H_

#include "messagemodel.h"

#include <QList>
#include "chatlinemodelitem.h"

class ChatLineModel : public MessageModel
{
    Q_OBJECT

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
public:
    enum ChatLineRole {
        MsgLabelRole,
    };

    ChatLineModel(QObject *parent = 0);

    virtual inline const MessageModelItem *messageItemAt(int i) const { return &_messageList[i]; }
protected:
    virtual inline int messageCount() const { return _messageList.count(); }
    virtual inline bool messagesIsEmpty() const { return _messageList.isEmpty(); }
    virtual inline MessageModelItem *messageItemAt(int i) { return &_messageList[i]; }
    virtual inline const MessageModelItem *firstMessageItem() const { return &_messageList.first(); }
    virtual inline MessageModelItem *firstMessageItem() { return &_messageList.first(); }
    virtual inline const MessageModelItem *lastMessageItem() const { return &_messageList.last(); }
    virtual inline MessageModelItem *lastMessageItem() { return &_messageList.last(); }
    virtual inline void insertMessage__(int pos, const Message &msg) { _messageList.insert(pos, ChatLineModelItem(msg)); }
    virtual void insertMessages__(int pos, const QList<Message> &);
    virtual inline void removeMessageAt(int i) { _messageList.removeAt(i); }
    virtual inline void removeAllMessages() { _messageList.clear(); }
    virtual Message takeMessageAt(int i);

protected slots:
    virtual void styleChanged();

private:
    QList<ChatLineModelItem> _messageList;
};

#endif
