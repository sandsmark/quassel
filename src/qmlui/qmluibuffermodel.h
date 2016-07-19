#ifndef QMLUIBUFFERMODEL_H_
#define QMLUIBUFFERMODEL_H_

#include "buffermodel.h"

class ClientBufferViewConfig;

class QmlUiBufferModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)

public:
    QmlUiBufferModel(QObject *parent = 0);

    // QAbstractListModel
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray>roleNames() const override;

    void setConfig(ClientBufferViewConfig *config);

Q_SIGNALS:
    void connectedChanged();
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

protected slots:

private:
    bool connected() const;
    bool bufferIdLessThan(const BufferId &left, const BufferId &right) const;
    bool bufferLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

    ClientBufferViewConfig *m_config;
};

#endif
