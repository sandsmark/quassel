#include "qmluiaccountmodel.h"

#include "client.h"

QmlUiAccountModel::QmlUiAccountModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, SIGNAL(modelReset()), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int)), SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int)), SIGNAL(countChanged()));
    setDynamicSortFilter(true);
    setSourceModel(Client::coreAccountModel());
    settings.notify("LastAccount", this, SIGNAL(lastAccountIdChanged()));
    if (count())
        Q_EMIT countChanged();
}


QVariant QmlUiAccountModel::data(const QModelIndex &index, int role) const
{
    QModelIndex sourceIndex(mapToSource(index));
    CoreAccount account(dynamic_cast<CoreAccountModel *>(sourceModel())->account(sourceIndex));
    switch(role) {
    case AccountIdRole:
        return QVariant::fromValue(account.accountId());
    case AccountNameRole:
        return account.accountName();
    case HostnameRole:
        return account.hostName();
    case UserRole:
        return account.user();
    case PasswordRole:
        return account.password();
    case PortRole:
        return account.port();
    case UseSecureConnectionRole:
        return true;
//        return account.useSsl();
    default:
        return QVariant();
    }
}


QHash<int, QByteArray> QmlUiAccountModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(AccountIdRole, "accountId");
    roles.insert(AccountNameRole, "accountName");
    roles.insert(HostnameRole, "hostname");
    roles.insert(UserRole, "user");
    roles.insert(PasswordRole, "password");
    roles.insert(PortRole, "port");
    roles.insert(UseSecureConnectionRole, "useSecureConnection");
    return roles;
}

QVariantMap
QmlUiAccountModel::get(int row)
{
    QVariantMap res;
    const QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> i(roles);
    while (i.hasNext()) {
        i.next();
        res.insert(i.value(), data(index(row, 0), i.key()));
    }
    return res;
}

bool QmlUiAccountModel::createOrUpdateAccount(QVariantMap accountMap)
{
    for (const QByteArray roleName : {"AccountName", "HostName", "Port", "User", "Password"}) {
        if (!accountMap.contains(roleName) || accountMap[roleName] == "") {
            emit error("Account info missing " + roleName);
            qWarning() << "Account info missing" << roleName;
            return false;
        }
    }

    CoreAccount account;
    account.fromVariantMap(accountMap);
    CoreAccountModel *model(dynamic_cast<CoreAccountModel *>(sourceModel()));
    AccountId accountId(model->createOrUpdateAccount(account));
    model->save();
    setAccountId(accountId.toInt());

    return true;
}
