/***************************************************************************
 *   Copyright (C) 2005-2011 by the Quassel Project                        *
 *   devel@quassel-irc.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) version 3.                                           *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qmlmessagemodel.h"

QmlMessageModel::QmlMessageModel(QObject *parent)
    : MessageModel(parent)
{
}

void QmlMessageModel::insertMessages__(int pos, const QList<Message> &messages) {
    for(int i = 0; i < messages.count(); i++) {
        _messageList.insert(pos, UiStyle::StyledMessage(messages[i]));
        pos++;
    }
}

Message QmlMessageModel::takeMessageAt(int i) {
    QmlMessageModelItem msg = _messageList.takeAt(i);
    return msg.message();
}

QVariant QmlMessageModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    if(row < 0 || row >= messageCount())
        return QVariant();

    const Message message = _messageList[row].message();
    switch (role) {
    case TimestampRole:
        return message.timestamp();
    case SenderRole:
        return message.sender();
    case ContentsRole:
        return message.contents();
    }

    return QVariant();
}

QHash<int, QByteArray> QmlMessageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TimestampRole] = "timestamp";
    roles[SenderRole] = "sender";
    roles[ContentsRole] = "contents";
    return roles;
}
