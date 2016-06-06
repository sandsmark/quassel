#include "qmluiaccountmodel.h"

#include "client.h"

QmlUiAccountModel::QmlUiAccountModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSourceModel(Client::coreAccountModel());
}


QVariant QmlUiAccountModel::data(const QModelIndex &index, int role) const
{
    QModelIndex sourceIndex(mapToSource(index));
    CoreAccount account(dynamic_cast<CoreAccountModel *>(sourceModel())->account(sourceIndex));
    switch(role) {
    case AccountIdRole:
        return account.accountId().toInt();
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
        return account.useSsl();
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
