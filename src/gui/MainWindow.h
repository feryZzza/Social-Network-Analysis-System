#ifndef GUI_MAINWINDOW_H
#define GUI_MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem> // 显式包含
#include <QVector>

#include "manager/core.h"

class AddPostDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default; // 添加析构函数

private slots:
    void onClientSelectionChanged();
    void onPostSelectionChanged();
    void handleLoadData();
    void handleSaveData();
    void handleRegisterClient();
    void handleOpenAddPostDialog();
    void handleDeletePost();
    void handleLikePost();
    void handleAnalyzePost(); // [新增] 处理哈夫曼分析
    void handleAddComment();
    void handleUndo();
    void handleReadMessages();
    void handleRefresh();
    void handleAddFriend();
    void handleRemoveFriend();

private:
    void buildUi();
    void refreshClients();
    void refreshPosts();
    void refreshFriends();
    void updateCurrentClientLabel();
    
    // 辅助函数：生成和解析帖子唯一键
    QString makePostKey(Client* owner, const Post& post) const;
    bool decodePostKey(const QString& key, QString& ownerName, int& idex) const;
    
    Post* resolvePostFromItem(QListWidgetItem* item) const;
    Post* findPost(const QString& ownerName, int idex) const;
    
    void showPostDetails(Post* post);
    void refreshComments(Post* post);
    void showStatusMessage(const QString& message, bool isError = false);
    void updateActionButtons();
    CoreStatus requireActiveClient() const;
    QString describeMessage(massege* msg) const;
    void applyTheme();

    Core& core;
    Client* currentClient = nullptr;

    QListWidget* clientList = nullptr;
    QListWidget* postList = nullptr;
    QListWidget* commentList = nullptr;
    QListWidget* friendList = nullptr;
    
    QLabel* postTitleLabel = nullptr;
    QLabel* postMetaLabel = nullptr;
    QLabel* postStatsLabel = nullptr;
    QTextEdit* postContentView = nullptr;
    QTextEdit* commentInput = nullptr;
    QLabel* currentUserLabel = nullptr;

    QPushButton* addPostButton = nullptr;
    QPushButton* deletePostButton = nullptr;
    QPushButton* likePostButton = nullptr;
    QPushButton* analyzePostButton = nullptr; // [新增] 分析按钮
    QPushButton* addCommentButton = nullptr;
    QPushButton* undoButton = nullptr;
    QPushButton* readMessagesButton = nullptr;
    QPushButton* addFriendButton = nullptr;
    QPushButton* removeFriendButton = nullptr;
};

#endif // GUI_MAINWINDOW_H