#include "ui/delegates/post_item_delegate.h"

#include <QPainter>
#include <QApplication>
#include <QAbstractItemView>

PostItemDelegate::PostItemDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

QSize PostItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(index);
    return {option.rect.width(), 72};
}

void PostItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    QRect r = option.rect.adjusted(8, 6, -8, -6);
    // 背景
    QColor bg = option.state & QStyle::State_Selected ? QColor("#eef2ff") : QColor("#ffffff");
    painter->fillRect(option.rect, bg);
    // 边框
    painter->setPen(QColor("#e5e7eb"));
    painter->drawRect(option.rect.adjusted(0, 0, -1, -1));

    // 取列：0 标题, 1 作者, 2 点赞, 3 评论
    QString title = index.sibling(index.row(), 0).data(Qt::DisplayRole).toString();
    QString author = index.sibling(index.row(), 1).data(Qt::DisplayRole).toString();
    int likes = index.sibling(index.row(), 2).data(Qt::DisplayRole).toInt();
    int comments = index.sibling(index.row(), 3).data(Qt::DisplayRole).toInt();

    // 标题行
    painter->setPen(QColor("#111827"));
    QFont f = painter->font();
    f.setBold(true);
    painter->setFont(f);
    painter->drawText(r.left(), r.top() + 18, title);

    // 元信息与徽章
    painter->setPen(QColor("#6b7280"));
    painter->setFont(QFont());
    QString meta = QString("%1   ·   赞 %2   ·   评论 %3").arg(author).arg(likes).arg(comments);
    painter->drawText(r.left(), r.top() + 38, meta);

    painter->restore();
}


