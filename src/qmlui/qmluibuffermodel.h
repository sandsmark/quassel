#ifndef QMLUIBUFFERMODEL_H_
#define QMLUIBUFFERMODEL_H_

#include "buffermodel.h"

class QmlUiBufferModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    QmlUiBufferModel(QObject *parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;

Q_SIGNALS:
    void connectedChanged();
protected:

protected slots:

private:
    bool connected() const;
};

#endif
