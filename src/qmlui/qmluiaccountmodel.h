#ifndef QMLUIACCOUNTMODEL_H_
#define QMLUIACCOUNTMODEL_H_

#include "coreaccountmodel.h"
#include "clientsettings.h"
#include "client.h"

#include <QSortFilterProxyModel>

class QmlUiAccountModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int lastAccountId READ lastAccountId WRITE setAccountId NOTIFY lastAccountIdChanged)
    // QML ListModel
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    QmlUiAccountModel(QObject *parent = 0);

    enum {
        AccountIdRole = Qt::UserRole,
        AccountNameRole,
        HostnameRole,
        UserRole,
        PasswordRole,
        PortRole,
        UseSecureConnectionRole,
        // TODO: Proxy
    };

    // QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    // QML ListModel
    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE int count() { return rowCount(); }

    Q_INVOKABLE void createOrUpdateAccount(QVariantMap accountMap) {
        CoreAccount account;
        account.fromVariantMap(accountMap);
        CoreAccountModel *model(dynamic_cast<CoreAccountModel *>(sourceModel()));
        AccountId accountId(model->createOrUpdateAccount(account));
        model->save();
        setAccountId(accountId.toInt());
    }

    Q_INVOKABLE void removeAccount(int accountId) {
        CoreAccountModel *model(dynamic_cast<CoreAccountModel *>(sourceModel()));
        model->removeAccount(accountId);
        model->save();
    }

    Q_INVOKABLE int indexOfLastAccount() {
        const QList<AccountId> accounts = Client::coreAccountModel()->accountIds();
        return accounts.indexOf(settings.lastAccount());
    }

Q_SIGNALS:
    void lastAccountIdChanged();
    void countChanged();

protected:

protected slots:
    void reconnect() {
        Client::coreConnection()->connectToCore(settings.lastAccount());
        CoreAccountModel *model(dynamic_cast<CoreAccountModel *>(sourceModel()));
        model->save();
    }

private:
    CoreAccountSettings settings;
    inline int lastAccountId() const {
        CoreAccount account(Client::currentCoreAccount());
        return account.isValid() ? account.accountId().toInt() : -1;
    }
    void setAccountId(int accountId) {
        if (lastAccountId() == accountId)
            return;
        Client::coreConnection()->disconnectFromCore();
        settings.setLastAccount(accountId);
        settings.setAutoConnectAccount(accountId);
        settings.setAutoConnectOnStartup(true);
        QTimer::singleShot(0, this, SLOT(reconnect()));
    }
};

#endif
