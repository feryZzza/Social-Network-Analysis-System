#ifndef POST_ITEM_DELEGATE_H
#define POST_ITEM_DELEGATE_H

#include <QStyledItemDelegate>

class PostItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit PostItemDelegate(QObject* parent = nullptr);
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // POST_ITEM_DELEGATE_H


