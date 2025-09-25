#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTableView;
class QStandardItemModel;
class ClientsTableModel;
class PostsTableModel;
class QToolBar;
class QAction;
class QSplitter;
class QLabel;
class QListWidget;
class QPushButton;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;
private:
    QTableView* clientsView = nullptr;
    QTableView* postsView = nullptr;
    QStandardItemModel* clientsModel = nullptr;
    QStandardItemModel* postsModel = nullptr;
public:
    void setClientsModel(ClientsTableModel* model);
    void setPostsModel(PostsTableModel* model);
private:
    QToolBar* toolbar = nullptr;
    QAction* actNewPost = nullptr;
    QAction* actLike = nullptr;
    QAction* actComment = nullptr;
    QAction* actUndo = nullptr;
    QAction* actRefresh = nullptr;
    ClientsTableModel* clientsProxy = nullptr;
    PostsTableModel* postsProxy = nullptr;
    // 帖子详情区控件
    QWidget* postDetail = nullptr;
    QLabel* lblPostTitle = nullptr;
    QLabel* lblPostMeta = nullptr; // 作者/点赞/评论计数
    QListWidget* listComments = nullptr;
    QPushButton* btnLike = nullptr;
    QPushButton* btnAddComment = nullptr;
    // 顶部汇总栏
    QWidget* summaryBar = nullptr;
    QLabel* lblSummary = nullptr;
    void updateSummary();
    void rebuildPostsForClient(class Client* client);

private slots:
    void onNewPost();
    void onLike();
    void onComment();
    void onUndo();
    void onRefresh();
    void onPostSelectionChanged();
    void updatePostDetail();
};

#endif // MAINWINDOW_H


