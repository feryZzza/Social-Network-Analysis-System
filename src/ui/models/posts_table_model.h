#ifndef POSTS_TABLE_MODEL_H
#define POSTS_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <vector>

class Post;

class PostsTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit PostsTableModel(QObject* parent = nullptr);

    void setPosts(const std::vector<Post*>& posts);
    const std::vector<Post*>& posts() const { return posts_; }
    Post* postAt(int row) const;
    void refresh();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<Post*> posts_;
};

#endif // POSTS_TABLE_MODEL_H


