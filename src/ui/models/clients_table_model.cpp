#include "ui/models/clients_table_model.h"

#include <QString>

#include "models/clients.h"
#include "models/Post.h"

ClientsTableModel::ClientsTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void ClientsTableModel::setClients(const std::vector<Client*>& clients) {
    beginResetModel();
    clients_ = clients;
    endResetModel();
}

Client* ClientsTableModel::clientAt(int row) const {
    if (row < 0 || row >= static_cast<int>(clients_.size())) return nullptr;
    return clients_[row];
}

void ClientsTableModel::refresh() {
    beginResetModel();
    endResetModel();
}

int ClientsTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(clients_.size());
}

int ClientsTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return 3; // 昵称, ID, 帖子数
}

QVariant ClientsTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};
    Client* c = clients_.at(index.row());
    switch (index.column()) {
        case 0: return QString::fromStdString(c->Name());
        case 1: return QString::fromStdString("" /* 暂无 ID getter */);
        case 2: return c->posts.size();
        default: return {};
    }
}

QVariant ClientsTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return QStringLiteral("昵称");
            case 1: return QStringLiteral("ID");
            case 2: return QStringLiteral("帖子数");
            default: return {};
        }
    }
    return section + 1;
}


