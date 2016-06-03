#ifndef QTUIMESSAGEPROCESSOR_H_
#define QTUIMESSAGEPROCESSOR_H_

#include <QTimer>

#include "abstractmessageprocessor.h"

class QmlUiMessageProcessor : public AbstractMessageProcessor
{
    Q_OBJECT

public:
    enum Mode {
        TimerBased,
        Concurrent
    };

    QmlUiMessageProcessor(QObject *parent);

    inline bool isProcessing() const { return _processing; }
    inline Mode processMode() const { return _processMode; }

    void reset();

public slots:
    void process(Message &msg);
    void process(QList<Message> &msgs);

private slots:
    void processNextMessage();
    void nicksCaseSensitiveChanged(const QVariant &variant);
    void highlightListChanged(const QVariant &variant);
    void highlightNickChanged(const QVariant &variant);

private:
    void checkForHighlight(Message &msg);
    void startProcessing();

    QList<QList<Message> > _processQueue;
    QList<Message> _currentBatch;
    QTimer _processTimer;
    bool _processing;
    Mode _processMode;

    struct HighlightRule {
        QString name;
        bool isEnabled;
        Qt::CaseSensitivity caseSensitive;
        bool isRegExp;
        QString chanName;
        inline HighlightRule(const QString &name, bool enabled, Qt::CaseSensitivity cs, bool regExp, const QString &chanName)
            : name(name), isEnabled(enabled), caseSensitive(cs), isRegExp(regExp), chanName(chanName) {}
    };

    QList<HighlightRule> _highlightRules;
    NotificationSettings::HighlightNickType _highlightNick;
    bool _nicksCaseSensitive;
};


#endif
