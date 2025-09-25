#include "ui/mainwindow.h"

#include <QTabWidget>
#include <QListView>
#include <QTreeView>
#include <QTableView>
#include <QSplitter>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include "ui/delegates/post_item_delegate.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QToolBar>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>
#include <QFile>
#include <QIODevice>
#include "models/clients.h"
#include "models/Post.h"
#include "models/comment.h"
// 引入模型声明以进行上行转换
#include "ui/models/clients_table_model.h"
#include "ui/models/posts_table_model.h"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    // 顶部汇总栏（白色背景）
    summaryBar = new QWidget(central);
    summaryBar->setStyleSheet("background:#ffffff;border-bottom:1px solid #e5e7eb;");
    auto* summaryLayout = new QHBoxLayout(summaryBar);
    summaryLayout->setContentsMargins(12,8,12,8);
    lblSummary = new QLabel(summaryBar);
    lblSummary->setStyleSheet("color:#111827;font-size:14px;font-weight:500;");
    summaryLayout->addWidget(lblSummary);
    summaryLayout->addStretch();
    layout->addWidget(summaryBar);

    auto* tabs = new QTabWidget(central);
    tabs->setObjectName("tabs");

    // 用户列表
    clientsView = new QTableView(tabs);
    clientsModel = new QStandardItemModel(this);
    clientsModel->setHorizontalHeaderLabels({QStringLiteral("昵称"), QStringLiteral("ID"), QStringLiteral("帖子数")});
    clientsView->setModel(clientsModel);
    tabs->addTab(clientsView, QStringLiteral("用户"));

    auto* postsPage = new QWidget(tabs);
    auto* postsLayout = new QVBoxLayout(postsPage);
    auto* splitter = new QSplitter(Qt::Horizontal, postsPage);

    // 左侧帖子表格
    postsView = new QTableView(splitter);
    postsModel = new QStandardItemModel(this);
    postsModel->setHorizontalHeaderLabels({QStringLiteral("标题"), QStringLiteral("作者"), QStringLiteral("点赞"), QStringLiteral("评论")});
    postsView->setModel(postsModel);
    postsView->setItemDelegate(new PostItemDelegate(postsView));
    postsView->setWordWrap(true);
    postsView->verticalHeader()->setDefaultSectionSize(78);
    splitter->addWidget(postsView);

    // 右侧详情卡片
    postDetail = new QWidget(splitter);
    auto* detailLayout = new QVBoxLayout(postDetail);
    lblPostTitle = new QLabel(postDetail);
    lblPostTitle->setStyleSheet("font-size:18px;font-weight:600;margin:6px 0;");
    lblPostMeta = new QLabel(postDetail);
    lblPostMeta->setStyleSheet("color:#8a8f98;margin-bottom:6px;");
    listComments = new QListWidget(postDetail);
    auto* actionRow = new QWidget(postDetail);
    auto* actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(0,0,0,0);
    btnLike = new QPushButton(QStringLiteral("点赞"), actionRow);
    btnAddComment = new QPushButton(QStringLiteral("评论"), actionRow);
    actionLayout->addWidget(btnLike);
    actionLayout->addWidget(btnAddComment);
    actionLayout->addStretch();
    detailLayout->addWidget(lblPostTitle);
    detailLayout->addWidget(lblPostMeta);
    detailLayout->addWidget(listComments, 1);
    detailLayout->addWidget(actionRow);
    splitter->addWidget(postDetail);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);
    postsLayout->addWidget(splitter);
    postsPage->setLayout(postsLayout);
    tabs->addTab(postsPage, QStringLiteral("帖子"));

    layout->addWidget(tabs);
    setCentralWidget(central);
    setWindowTitle(QStringLiteral("社交网络管理系统"));
    resize(960, 640);

    // 工具栏与动作
    toolbar = addToolBar(QStringLiteral("操作"));
    actNewPost = toolbar->addAction(QStringLiteral("发帖"), this, &MainWindow::onNewPost);
    actLike = toolbar->addAction(QStringLiteral("点赞"), this, &MainWindow::onLike);
    actComment = toolbar->addAction(QStringLiteral("评论"), this, &MainWindow::onComment);
    actUndo = toolbar->addAction(QStringLiteral("撤销"), this, &MainWindow::onUndo);
    toolbar->addSeparator();
    actRefresh = toolbar->addAction(QStringLiteral("刷新"), this, &MainWindow::onRefresh);

    setStyleSheet("QWidget{background:#ffffff;color:#111827;} QTableView{alternate-background-color:#f9fafb;}");
    clientsView->setAlternatingRowColors(true);
    postsView->setAlternatingRowColors(true);
    clientsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    postsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    clientsView->setSelectionMode(QAbstractItemView::SingleSelection);
    postsView->setSelectionMode(QAbstractItemView::SingleSelection);
    clientsView->setShowGrid(false);
    postsView->setShowGrid(false);
    clientsView->horizontalHeader()->setStretchLastSection(true);
    postsView->horizontalHeader()->setStretchLastSection(true);
    clientsView->verticalHeader()->setVisible(false);
    postsView->verticalHeader()->setVisible(false);

    connect(postsView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::onPostSelectionChanged);
    connect(btnLike, &QPushButton::clicked, this, &MainWindow::onLike);
    connect(btnAddComment, &QPushButton::clicked, this, &MainWindow::onComment);
    updateSummary();
}

void MainWindow::setClientsModel(ClientsTableModel* model) {
    clientsView->setModel(model);
    clientsProxy = model;
    updateSummary();
}

void MainWindow::setPostsModel(PostsTableModel* model) {
    postsView->setModel(model);
    postsProxy = model;
    connect(postsView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::onPostSelectionChanged);
    updateSummary();
}

void MainWindow::onNewPost() {
    if (!clientsProxy) return;
    auto idx = clientsView->currentIndex();
    Client* c = clientsProxy->clientAt(idx.row());
    if (!c) { QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请先选择一个用户")); return; }
    bool ok1=false, ok2=false;
    QString title = QInputDialog::getText(this, QStringLiteral("新帖子"), QStringLiteral("标题"), QLineEdit::Normal, {}, &ok1);
    if (!ok1 || title.isEmpty()) return;
    QString content = QInputDialog::getMultiLineText(this, QStringLiteral("新帖子"), QStringLiteral("内容"), {}, &ok2);
    if (!ok2) return;
    Post p(title.toStdString(), content.toStdString());
    c->addPost(p);
    rebuildPostsForClient(c);
    updateSummary();
}

void MainWindow::onLike() {
    if (!clientsProxy || !postsProxy) return;
    Client* c = clientsProxy->clientAt(clientsView->currentIndex().row());
    Post* p = postsProxy->postAt(postsView->currentIndex().row());
    if (!c || !p) { QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请选择用户与帖子")); return; }
    c->like(p);
    rebuildPostsForClient(p->author);
    updateSummary();
}

void MainWindow::onComment() {
    if (!clientsProxy || !postsProxy) return;
    Client* commenter = clientsProxy->clientAt(clientsView->currentIndex().row());
    Post* p = postsProxy->postAt(postsView->currentIndex().row());
    if (!commenter || !p) { QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请选择用户与帖子")); return; }
    bool ok=false;
    QString text = QInputDialog::getMultiLineText(this, QStringLiteral("发表评论"), QStringLiteral("内容"), {}, &ok);
    if (!ok || text.isEmpty()) return;
    Comment c(commenter, text.toStdString());
    commenter->addComment(p, c);
    rebuildPostsForClient(p->author);
    updateSummary();
}

void MainWindow::onUndo() {
    if (!clientsProxy) return;
    Client* c = clientsProxy->clientAt(clientsView->currentIndex().row());
    if (!c) { QMessageBox::warning(this, QStringLiteral("提示"), QStringLiteral("请选择用户")); return; }
    if (!c->undo()) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("没有可撤销的操作"));
    }
    if (postsProxy) postsProxy->refresh();
    updatePostDetail();
    updateSummary();
}

void MainWindow::onRefresh() {
    if (clientsProxy) clientsProxy->refresh();
    if (postsProxy) postsProxy->refresh();
    updatePostDetail();
    updateSummary();
}

void MainWindow::updateSummary() {

    if (!clientsProxy || !lblSummary) return;
    int totalPosts = 0;
    int totalLikes = 0;
    int totalComments = 0;
    const auto& list = clientsProxy->clients();
    for (auto* c : list) {
        if (!c) continue;
        totalPosts += c->posts.size();
        for (int i = 0; i < c->posts.size(); ++i) {
            Post& p = c->posts[i];
            totalLikes += p.likes_num();
            totalComments += p.comments_num();
        }
    }
    lblSummary->setText(QStringLiteral("帖子：%1  ·  累计点赞：%2  ·  累计评论：%3")
        .arg(totalPosts).arg(totalLikes).arg(totalComments));
}

void MainWindow::onPostSelectionChanged() {
    updatePostDetail();
}

void MainWindow::updatePostDetail() {
    if (!postsProxy) return;
    Post* p = postsProxy->postAt(postsView->currentIndex().row());
    listComments->clear();
    if (!p) { lblPostTitle->clear(); lblPostMeta->clear(); return; }
    lblPostTitle->setText(QString::fromStdString(p->get_title()));
    QString meta = QStringLiteral("作者：%1  ·  赞 %2  ·  评论 %3")
        .arg(p->author ? QString::fromStdString(p->author->Name()) : QStringLiteral("-"))
        .arg(p->likes_num())
        .arg(p->comments_num());
    lblPostMeta->setText(meta);
    for (int i = 0; i < p->comment_list.size(); ++i) {
        Comment& c = p->comment_list[i];
        QString line = QStringLiteral("%1：%2")
            .arg(c.get_author() ? QString::fromStdString(c.get_author()->Name()) : QStringLiteral("-"))
            .arg(QString::fromStdString(c.get_content()));
        listComments->addItem(line);
    }
}

void MainWindow::rebuildPostsForClient(Client* client) {
    if (!client || !postsProxy) return;
    std::vector<Post*> postPtrs;
    for (int i = 0; i < client->posts.size(); ++i) {
        postPtrs.push_back(&client->posts[i]);
    }
    postsProxy->setPosts(postPtrs);
    updatePostDetail();
}



MainWindow::~MainWindow() = default;


