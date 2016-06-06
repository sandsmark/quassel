#ifndef QMLUIACCOUNTMODEL_H_
#define QMLUIACCOUNTMODEL_H_

#include "coreaccountmodel.h"
#include "clientsettings.h"

#include <QSortFilterProxyModel>

class QmlUiAccountModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int lastAccountId READ lastAccountId)

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

protected:

protected slots:

private:
    CoreAccountSettings settings;
    inline int lastAccountId() { return settings.lastAccount().toInt(); }
};

#endif
