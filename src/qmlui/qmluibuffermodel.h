#ifndef QMLUIBUFFERMODEL_H_
#define QMLUIBUFFERMODEL_H_

#include "buffermodel.h"

class QmlUiBufferModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    QmlUiBufferModel(QObject *parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QHash<int, QByteArray>roleNames() const;
protected:

protected slots:

private:
};

#endif
