#include "qmluimessageprocessor.h"

#include "client.h"
#include "clientsettings.h"
#include "identity.h"
#include "messagemodel.h"
#include "network.h"

QmlUiMessageProcessor::QmlUiMessageProcessor(QObject *parent)
    : AbstractMessageProcessor(parent),
    _processing(false),
    _processMode(TimerBased)
{
    NotificationSettings notificationSettings;
    _nicksCaseSensitive = notificationSettings.nicksCaseSensitive();
    _highlightNick = notificationSettings.highlightNick();
    highlightListChanged(notificationSettings.highlightList());
    notificationSettings.notify("Highlights/NicksCaseSensitive", this, SLOT(nicksCaseSensitiveChanged(const QVariant &)));
    notificationSettings.notify("Highlights/CustomList", this, SLOT(highlightListChanged(const QVariant &)));
    notificationSettings.notify("Highlights/HighlightNick", this, SLOT(highlightNickChanged(const QVariant &)));

    _processTimer.setInterval(0);
    connect(&_processTimer, SIGNAL(timeout()), this, SLOT(processNextMessage()));
}


void QmlUiMessageProcessor::reset()
{
    if (processMode() == TimerBased) {
        if (_processTimer.isActive()) _processTimer.stop();
        _processing = false;
        _currentBatch.clear();
        _processQueue.clear();
    }
}


void QmlUiMessageProcessor::process(Message &msg)
{
    checkForHighlight(msg);
    preProcess(msg);
    Client::messageModel()->insertMessage(msg);
}


void QmlUiMessageProcessor::process(QList<Message> &msgs)
{
    QList<Message>::iterator msgIter = msgs.begin();
    QList<Message>::iterator msgIterEnd = msgs.end();
    while (msgIter != msgIterEnd) {
        checkForHighlight(*msgIter);
        preProcess(*msgIter);
        ++msgIter;
    }
    Client::messageModel()->insertMessages(msgs);
    return;

    if (msgs.isEmpty()) return;
    _processQueue.append(msgs);
    if (!isProcessing())
        startProcessing();
}


void QmlUiMessageProcessor::startProcessing()
{
    if (processMode() == TimerBased) {
        if (_currentBatch.isEmpty() && _processQueue.isEmpty())
            return;
        _processing = true;
        if (!_processTimer.isActive())
            _processTimer.start();
    }
}


void QmlUiMessageProcessor::processNextMessage()
{
    if (_currentBatch.isEmpty()) {
        if (_processQueue.isEmpty()) {
            _processTimer.stop();
            _processing = false;
            return;
        }
        _currentBatch = _processQueue.takeFirst();
    }
    Message msg = _currentBatch.takeFirst();
    process(msg);
}


void QmlUiMessageProcessor::checkForHighlight(Message &msg)
{
    if (!((msg.type() & (Message::Plain | Message::Notice | Message::Action)) && !(msg.flags() & Message::Self)))
        return;

    // TODO: Cache this (per network)
    const Network *net = Client::network(msg.bufferInfo().networkId());
    if (net && !net->myNick().isEmpty()) {
        QStringList nickList;
        if (_highlightNick == NotificationSettings::CurrentNick) {
            nickList << net->myNick();
        }
        else if (_highlightNick == NotificationSettings::AllNicks) {
            const Identity *myIdentity = Client::identity(net->identity());
            if (myIdentity)
                nickList = myIdentity->nicks();
            if (!nickList.contains(net->myNick()))
                nickList.prepend(net->myNick());
        }
        foreach(QString nickname, nickList) {
            QRegExp nickRegExp("(^|\\W)" + QRegExp::escape(nickname) + "(\\W|$)", _nicksCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
            if (nickRegExp.indexIn(msg.contents()) >= 0) {
                msg.setFlags(msg.flags() | Message::Highlight);
                return;
            }
        }

        for (int i = 0; i < _highlightRules.count(); i++) {
            const HighlightRule &rule = _highlightRules.at(i);
            if (!rule.isEnabled)
                continue;

            if (rule.chanName.size() > 0 && rule.chanName.compare(".*") != 0) {
                if (rule.chanName.startsWith("!")) {
                    QRegExp rx(rule.chanName.mid(1), Qt::CaseInsensitive);
                    if (rx.exactMatch(msg.bufferInfo().bufferName()))
                        continue;
                }
                else {
                    QRegExp rx(rule.chanName, Qt::CaseInsensitive);
                    if (!rx.exactMatch(msg.bufferInfo().bufferName()))
                        continue;
                }
            }

            QRegExp rx;
            if (rule.isRegExp) {
                rx = QRegExp(rule.name, rule.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
            }
            else {
                rx = QRegExp("(^|\\W)" + QRegExp::escape(rule.name) + "(\\W|$)", rule.caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
            }
            bool match = (rx.indexIn(msg.contents()) >= 0);
            if (match) {
                msg.setFlags(msg.flags() | Message::Highlight);
                return;
            }
        }
    }
}


void QmlUiMessageProcessor::nicksCaseSensitiveChanged(const QVariant &variant)
{
    _nicksCaseSensitive = variant.toBool();
}


void QmlUiMessageProcessor::highlightListChanged(const QVariant &variant)
{
    QVariantList varList = variant.toList();

    _highlightRules.clear();
    QVariantList::const_iterator iter = varList.constBegin();
    while (iter != varList.constEnd()) {
        QVariantMap rule = iter->toMap();
        _highlightRules << HighlightRule(rule["Name"].toString(),
            rule["Enable"].toBool(),
            rule["CS"].toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive,
            rule["RegEx"].toBool(),
            rule["Channel"].toString());
        ++iter;
    }
}


void QmlUiMessageProcessor::highlightNickChanged(const QVariant &variant)
{
    _highlightNick = (NotificationSettings::HighlightNickType)variant.toInt();
}
