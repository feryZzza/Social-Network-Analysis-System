#ifndef CLIENTS_TABLE_MODEL_H
#define CLIENTS_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <vector>

class Client;

class ClientsTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ClientsTableModel(QObject* parent = nullptr);

    void setClients(const std::vector<Client*>& clients);
    const std::vector<Client*>& clients() const { return clients_ ; }
    Client* clientAt(int row) const;
    void refresh();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<Client*> clients_;
};

#endif // CLIENTS_TABLE_MODEL_H


