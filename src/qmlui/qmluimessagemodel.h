#ifndef QMLUIMESSAGEMODEL_H_
#define QMLUIMESSAGEMODEL_H_

#include "messagemodel.h"

#include <QSortFilterProxyModel>

class QmlUiMessageModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(int bufferId READ bufferId WRITE setBufferId NOTIFY bufferIdChanged)

public:
    QmlUiMessageModel(QObject *parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
    // QSortFilterProxyModel
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

Q_SIGNALS:
    void bufferIdChanged();

protected:

protected slots:

private:
    inline int bufferId() const { return _bufferId; }
    inline void setBufferId(int bufferId) { _bufferId = bufferId; invalidateFilter(); Q_EMIT bufferIdChanged(); }
    int _bufferId;
    // TODO: Expose property
    bool _showMeta;
};

#endif
