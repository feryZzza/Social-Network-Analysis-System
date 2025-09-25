#include "ui/models/posts_table_model.h"

#include <QString>

#include "models/Post.h"
#include "models/clients.h"

PostsTableModel::PostsTableModel(QObject* parent)
    : QAbstractTableModel(parent) {}

void PostsTableModel::setPosts(const std::vector<Post*>& posts) {
    beginResetModel();
    posts_ = posts;
    endResetModel();
}

Post* PostsTableModel::postAt(int row) const {
    if (row < 0 || row >= static_cast<int>(posts_.size())) return nullptr;
    return posts_[row];
}

void PostsTableModel::refresh() {
    beginResetModel();
    endResetModel();
}

int PostsTableModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(posts_.size());
}

int PostsTableModel::columnCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return 4; // 标题, 作者, 点赞, 评论
}

QVariant PostsTableModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) return {};
    Post* p = posts_.at(index.row());
    switch (index.column()) {
        case 0: return QString::fromStdString(p->get_title());
        case 1: return p->author ? QString::fromStdString(p->author->Name()) : QString();
        case 2: return p->likes_num();
        case 3: return p->comments_num();
        default: return {};
    }
}

QVariant PostsTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return {};
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return QStringLiteral("标题");
            case 1: return QStringLiteral("作者");
            case 2: return QStringLiteral("点赞");
            case 3: return QStringLiteral("评论");
            default: return {};
        }
    }
    return section + 1;
}


