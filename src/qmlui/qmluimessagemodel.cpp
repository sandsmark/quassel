#include "qmluimessagemodel.h"

#include <QRegularExpression>

#include "client.h"
#include "buffersettings.h"
#include "clientignorelistmanager.h"

QmlUiMessageModel::QmlUiMessageModel(QObject *parent)
    : QSortFilterProxyModel(parent),
    _networkId(0),
    _network(0)
{
    setDynamicSortFilter(true);
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(messagesInserted(const QModelIndex&,int,int)));
    setFilterRole(MessageModel::BufferIdRole);
    connect(Client::backlogManager(), SIGNAL(updateProgress(int,int)), this, SIGNAL(backlogProgressChanged()));
    setSourceModel(Client::messageModel());
}


void QmlUiMessageModel::messagesInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);

    for (int i = start; i <= end; i++) {
        QModelIndex idx(index(i, MessageModel::ContentsColumn));
        if (!idx.isValid()) {
            qCritical() << "Invalid model index!";
            continue;
        }

        Q_EMIT countChanged();

        Message::Flags flags = (Message::Flags)idx.data(MessageModel::FlagsRole).toInt();
        if (flags.testFlag(Message::Backlog)) {
            Q_EMIT backlogReceived();
            continue;
        }
        if (flags.testFlag(Message::Self))
            continue;

        BufferId bufId = idx.data(MessageModel::BufferIdRole).value<BufferId>();
        BufferInfo::Type bufType = Client::networkModel()->bufferType(bufId);

        // check if bufferId belongs to the shown chatlists
        /* if (!(Client::bufferViewOverlay()->bufferIds().contains(bufId) ||
              Client::bufferViewOverlay()->tempRemovedBufferIds().contains(bufId)))
            continue; */

        // check if it's the buffer currently displayed
        if (bufId == _bufferId)
            continue;

        // only show notifications for higlights or queries
        if (bufType != BufferInfo::QueryBuffer && !(flags & Message::Highlight))
            continue;

        // and of course: don't notify for ignored messages
        if (Client::ignoreListManager() && Client::ignoreListManager()->match(idx.data(MessageModel::MessageRole).value<Message>(), Client::networkModel()->networkName(bufId)))
            continue;

        QModelIndex senderIdx (index(i, MessageModel::SenderColumn));
        QString sender(senderIdx.data(MessageModel::EditRole).toString());
        QString contents(idx.data(MessageModel::DisplayRole).toString());
        Q_EMIT highlightReceived(sender, contents);
    }
}


QVariant QmlUiMessageModel::data(const QModelIndex &index, int role) const
{
    QModelIndex sourceIndex(mapToSource(index));
    Message message(sourceModel()->data(sourceIndex, MessageModel::MessageRole).value<Message>());
    switch(role) {
    case MessageModel::UserRole:
        return message.sender();
    case MessageModel::MessageRole:
        return stripFormatCodes(message.contents());
    case StyledMessageRole:
    {
        // cf. src/uisupport/uistyle.cpp
        QString text(message.contents()), nick(nickFromMask(message.sender()).replace('%', "%%"));
        QString user(userFromMask(message.sender()).replace('%', "%%")), host(hostFromMask(message.sender()).replace('%', "%%")), buffer(bufferName().replace('%', "%%"));
        switch(message.type()) {
            case Message::Nick:
                text = text == nick ? tr("You are now known as %DN%1%DN").arg(text) : tr("%DN%1%DN is now known as %DN%2%DN").arg(nick).arg(text);
                break;
            case Message::Mode:
                text = nick.isEmpty() ? tr("User mode: %DM%1%DM").arg(text) : tr("Mode %DM%1%DM by %DN%2%DN").arg(text, nick);
                break;
            case Message::Join:
                text = tr("%DN%1%DN %DH(%2@%3)%DH has joined %DC%4%DC").arg(nick, user, host, buffer);
                break;
            case Message::Part:
                text = tr("%DN%1%DN %DH(%2@%3)%DH has left %DC%4%DC").arg(nick, user, host, buffer) + (text.isEmpty() ? "" : QString(" (%1)").arg(text));
                break;
            case Message::Quit:
                if (text.isEmpty())
                    text = tr("%DN%1%DN %DH(%2@%3)%DH has quit").arg(nick, user, host);
                break;
            case Message::Kick:
            {
                QString victim(text.section(" ", 0, 0)), kickMessage(text.section(" ", 1));
                text = tr("%DN%1%DN has kicked %DN%2%DN from %DC%3%DC").arg(nick, victim, buffer) + (kickMessage.isEmpty() ? "" : QString(" (%1)").arg(kickMessage));
                break;
            }
            case Message::Kill:
            case Message::Server:
            case Message::Info:
            case Message::Error:
                break;
            case Message::DayChange:
                text = tr("{Day changed to %1}").arg(message.timestamp().date().toString(Qt::DefaultLocaleLongDate));
            case Message::Topic:
                break;
            case Message::NetsplitJoin:
            case Message::NetsplitQuit:
            {
                QStringList users(text.split("#:#"));
                QStringList servers(users.takeLast().split(" "));
                const int maxNetsplitNicks = 15;
                for (int i = 0; i < users.count() && i < maxNetsplitNicks; i++)
                    users[i] = nickFromMask(users.at(i));
                text = (message.type() == Message::NetsplitJoin ? tr("Netsplit between %DH%1%DH and %DH%2%DH ended. Users joined: ") : tr("Netsplit between %DH%1%DH and %DH%2%DH. Users quit: ")).arg(servers.at(0), servers.at(1));
                if (users.count() <= maxNetsplitNicks)
                    text.append(QString("%DN%1%DN").arg(users.join(", ")));
                else
                    text.append(tr("%DN%1%DN (%2 more)").arg(static_cast<QStringList>(users.mid(0, maxNetsplitNicks)).join(", ")).arg(users.count() - maxNetsplitNicks));
            }
            case Message::Invite:
            case Message::Plain:
            case Message::Notice:
            case Message::Action:
                break;
        }
        // Escape
        text.replace(QRegExp("&(?!amp;)"), "&amp;");
        text.replace(QRegExp("<"), "&lt;");
        text.replace(QRegExp(">"), "&gt;");
        text.replace(QRegExp("\""), "&quot;");
        // Inline images: before
        text.replace(QRegExp("(https?)(://)([^\\s]+)(\\.png|jpe?g|gif)"), "<img src=\"\\1-//\\3\\4\">");
        // URLs
        text.replace(QRegExp("(https?://[^\\s]+)"), "<a href=\"\\1\">\\1</a>");
        // Inline images: after
        text.replace(QRegExp("(https?)(-//)([^\\s]+)(\\.png|jpe?g|gif)"), "\\1://\\3\\4");
        // UIStyle
        text.replace("%%", "%");
        text.replace(QRegularExpression("%D[NHCMU](.*?)%D[NHCMU]"), "<i>\\1</i>");
        // mIRC
        text.replace(QRegularExpression("\x02(.*?)\x02"), "<b>\\1</b>");
        text.replace(QRegularExpression("\x1d(.*?)\x1d"), "<s>\\1</s>");
        text.replace(QRegularExpression("\x1f(.*?)\x1f"), "<u>\\1</u>");
        // Strip codes we don't handle
        return stripFormatCodes(text);
    }
    case MessageModel::TimestampRole:
        return message.timestamp();
    case PreviousSiblingRole:
    {
        // Does the previous row share the same sender?
        QModelIndex previous(index.sibling(index.row() - 1, index.column()));
        if (!previous.isValid())
            return false;
        return previous.data(MessageModel::UserRole) == index.data(MessageModel::UserRole);
    }
    case NextSiblingRole:
    {
        // Does the next row share the same sender?
        QModelIndex next(index.sibling(index.row() + 1, index.column()));
        if (!next.isValid())
            return false;
        return next.data(MessageModel::UserRole) == index.data(MessageModel::UserRole);
    }
    case HighlightRole:
        return QVariant::fromValue<bool>(message.flags() & Message::Highlight);
    case SelfRole:
        return QVariant::fromValue<bool>(message.flags() & Message::Self);
    case ActionRole:
        switch (message.type()) {
        case Message::Action:
            return true;
        case Message::Quit:
            return !message.contents().isEmpty();
        default:
            return false;
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
    roles.insert(StyledMessageRole, "styledMessage");
    roles.insert(MessageModel::TimestampRole, "timestamp");
    roles.insert(PreviousSiblingRole, "previousSibling");
    roles.insert(NextSiblingRole, "nextSibling");
    roles.insert(HighlightRole, "highlight");
    roles.insert(SelfRole, "self");
    roles.insert(ActionRole, "action");
    roles.insert(MessageModel::MsgIdRole, "msgId");
    return roles;
}


bool QmlUiMessageModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex(sourceModel()->index(sourceRow, 0, sourceParent));
    Message message(sourceIndex.data(MessageModel::MessageRole).value<Message>());
    if (BufferSettings().messageFilter() & message.type())
        return false;
    return message.bufferId().toInt() == _bufferId;
}

void QmlUiMessageModel::userInput(const QString& message)
{
    Client::userInput(Client::networkModel()->bufferInfo(_bufferId), message);
}
