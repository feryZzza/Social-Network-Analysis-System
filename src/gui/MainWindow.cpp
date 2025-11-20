#include "MainWindow.h"
#include "AddPostDialog.h"

#include <QAbstractItemView>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QList>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStringList>
#include <QTextCursor>
#include <QVector>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QDialog>
#include <QHeaderView>
#include <QTableWidget>
#include <string>

namespace {

QString statusToText(CoreStatus status) {
    switch (status) {
    case SUCCESS: return QObject::tr("操作成功");
    case ERR_CLIENT_EXISTS: return QObject::tr("用户已存在 (昵称冲突)"); // 明确提示是昵称冲突
    case ERR_LIST_FULL: return QObject::tr("用户列表已满");
    case ERR_CLIENT_NOT_FOUND: return QObject::tr("未找到用户");
    case ERR_POST_NOT_FOUND: return QObject::tr("未找到帖子");
    case ERR_COMMENT_NOT_FOUND: return QObject::tr("未找到评论");
    case ERR_NO_ACTION_TO_UNDO: return QObject::tr("没有可撤销的操作");
    case ERR_ACTION_INVALID: return QObject::tr("操作已失效");
    case ERR_ALREADY_FRIENDS: return QObject::tr("已经是好友了");
    case ERR_SELF_FRIEND: return QObject::tr("不能添加自己为好友");
    default: return QObject::tr("未知错误");
    }
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , core(Core::instance()) { 
    setWindowTitle(tr("社交网络管理系统 - Qt GUI"));
    resize(1280, 720);
    buildUi();
    applyTheme();

    bool loaded = core.loadData();
    if (loaded) {
        showStatusMessage(tr("成功加载历史数据。"));
    } else {
        showStatusMessage(tr("没有找到历史数据，已初始化空数据。"));
    }

    refreshClients();
    refreshPosts();
}

void MainWindow::buildUi() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, central);

    // === Left panel ===
    QWidget* leftPanel = new QWidget(splitter);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    QLabel* heroTitle = new QLabel(tr("社交网络控制台"), leftPanel);
    heroTitle->setObjectName("heroTitle");
    QLabel* heroSubtitle = new QLabel(tr("管理成员、帖子与好友关系"), leftPanel);
    heroSubtitle->setObjectName("heroSubtitle");

    clientList = new QListWidget(leftPanel);
    clientList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(clientList, &QListWidget::itemSelectionChanged, this, &MainWindow::onClientSelectionChanged);

    QPushButton* loadButton = new QPushButton(tr("加载数据"), leftPanel);
    QPushButton* saveButton = new QPushButton(tr("保存数据"), leftPanel);
    QPushButton* registerButton = new QPushButton(tr("注册新用户"), leftPanel);
    QPushButton* refreshButton = new QPushButton(tr("刷新列表"), leftPanel);

    connect(loadButton, &QPushButton::clicked, this, &MainWindow::handleLoadData);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::handleSaveData);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::handleRegisterClient);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::handleRefresh);

    friendList = new QListWidget(leftPanel);
    friendList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(friendList, &QListWidget::itemSelectionChanged, this, [this]() {
        updateActionButtons();
    });
    addFriendButton = new QPushButton(tr("添加好友"), leftPanel);
    removeFriendButton = new QPushButton(tr("删除好友"), leftPanel);
    connect(addFriendButton, &QPushButton::clicked, this, &MainWindow::handleAddFriend);
    connect(removeFriendButton, &QPushButton::clicked, this, &MainWindow::handleRemoveFriend);
    QHBoxLayout* friendButtonsLayout = new QHBoxLayout();
    friendButtonsLayout->addWidget(addFriendButton);
    friendButtonsLayout->addWidget(removeFriendButton);

    currentUserLabel = new QLabel(tr("当前用户：未选择"), leftPanel);
    currentUserLabel->setObjectName("currentUserLabel");

    QGroupBox* clientBox = new QGroupBox(tr("用户列表"), leftPanel);
    QVBoxLayout* clientBoxLayout = new QVBoxLayout(clientBox);
    clientBoxLayout->addWidget(clientList);
    QHBoxLayout* dataButtonsLayout = new QHBoxLayout();
    dataButtonsLayout->addWidget(loadButton);
    dataButtonsLayout->addWidget(saveButton);
    clientBoxLayout->addLayout(dataButtonsLayout);
    QHBoxLayout* accountButtonsLayout = new QHBoxLayout();
    accountButtonsLayout->addWidget(registerButton);
    accountButtonsLayout->addWidget(refreshButton);
    clientBoxLayout->addLayout(accountButtonsLayout);

    QGroupBox* friendBox = new QGroupBox(tr("好友列表"), leftPanel);
    QVBoxLayout* friendBoxLayout = new QVBoxLayout(friendBox);
    friendBoxLayout->addWidget(friendList);
    friendBoxLayout->addLayout(friendButtonsLayout);

    QPushButton* userRankingButton = new QPushButton(tr("用户影响力榜"), leftPanel);
    QPushButton* postRankingButton = new QPushButton(tr("热门帖子榜"), leftPanel);
    QPushButton* relationChainButton = new QPushButton(tr("好友关系链"), leftPanel);
    connect(userRankingButton, &QPushButton::clicked, this, &MainWindow::handleShowUserRanking);
    connect(postRankingButton, &QPushButton::clicked, this, &MainWindow::handleShowHotPostRanking);
    connect(relationChainButton, &QPushButton::clicked, this, &MainWindow::handleShowRelationChain);
    QGroupBox* rankingBox = new QGroupBox(tr("排行榜"), leftPanel);
    QVBoxLayout* rankingLayout = new QVBoxLayout(rankingBox);
    rankingLayout->addWidget(userRankingButton);
    rankingLayout->addWidget(postRankingButton);
    rankingLayout->addWidget(relationChainButton);

    leftLayout->addWidget(heroTitle);
    leftLayout->addWidget(heroSubtitle);
    leftLayout->addWidget(currentUserLabel);
    leftLayout->addWidget(clientBox);
    leftLayout->addWidget(friendBox);
    leftLayout->addWidget(rankingBox);
    leftLayout->addStretch(1);

    // === Right panel ===
    QWidget* rightPanel = new QWidget(splitter);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    const QString postSectionTitle = tr("帖子列表");
    postList = new QListWidget(rightPanel);
    postList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(postList, &QListWidget::itemSelectionChanged, this, &MainWindow::onPostSelectionChanged);

    QHBoxLayout* postActionLayout = new QHBoxLayout();
    postActionLayout->setSpacing(12);
    addPostButton = new QPushButton(tr("发布新帖子..."), rightPanel);
    deletePostButton = new QPushButton(tr("删除帖子"), rightPanel);
    likePostButton = new QPushButton(tr("点赞 / 取消赞"), rightPanel);
    analyzePostButton = new QPushButton(tr("内容分析 (Huffman)"), rightPanel); // [新增]
    undoButton = new QPushButton(tr("撤销上一步"), rightPanel);
    readMessagesButton = new QPushButton(tr("读取消息"), rightPanel);

    connect(addPostButton, &QPushButton::clicked, this, &MainWindow::handleOpenAddPostDialog);
    connect(deletePostButton, &QPushButton::clicked, this, &MainWindow::handleDeletePost);
    connect(likePostButton, &QPushButton::clicked, this, &MainWindow::handleLikePost);
    connect(analyzePostButton, &QPushButton::clicked, this, &MainWindow::handleAnalyzePost); // [新增]
    connect(undoButton, &QPushButton::clicked, this, &MainWindow::handleUndo);
    connect(readMessagesButton, &QPushButton::clicked, this, &MainWindow::handleReadMessages);

    addPostButton->setProperty("accent", true);
    deletePostButton->setProperty("danger", true);
    likePostButton->setProperty("ghost", true);
    analyzePostButton->setProperty("ghost", true); // 设置样式
    undoButton->setProperty("ghost", true);
    readMessagesButton->setProperty("ghost", true);

    postActionLayout->addWidget(addPostButton);
    postActionLayout->addWidget(deletePostButton);
    postActionLayout->addWidget(likePostButton);
    postActionLayout->addWidget(analyzePostButton); // [新增]
    postActionLayout->addWidget(undoButton);
    postActionLayout->addWidget(readMessagesButton);

    QGroupBox* feedBox = new QGroupBox(postSectionTitle, rightPanel);
    QVBoxLayout* feedLayout = new QVBoxLayout(feedBox);
    feedLayout->addWidget(postList);
    feedLayout->addLayout(postActionLayout);

    postTitleLabel = new QLabel(tr("选择一个帖子查看详情"), rightPanel);
    postTitleLabel->setObjectName("postTitle");
    postMetaLabel = new QLabel(tr("在左侧选择帖子以查看楼主信息"), rightPanel);
    postMetaLabel->setObjectName("postMeta");
    postStatsLabel = new QLabel(tr("👍 0  |  💬 0  |  楼层 0"), rightPanel);
    postStatsLabel->setObjectName("postStats");

    postContentView = new QTextEdit(rightPanel);
    postContentView->setReadOnly(true);
    postContentView->setPlaceholderText(tr("帖子内容将显示在此处"));
    postContentView->setObjectName("postContentBox");
    postContentView->setMinimumHeight(200);

    commentList = new QListWidget(rightPanel);
    commentList->setSelectionMode(QAbstractItemView::NoSelection);
    commentList->setSpacing(8);
    commentList->setWordWrap(true);
    commentList->setObjectName("commentList");

    QGroupBox* detailBox = new QGroupBox(tr("帖子详情与评论"), rightPanel);
    QVBoxLayout* detailLayout = new QVBoxLayout(detailBox);
    QHBoxLayout* detailRow = new QHBoxLayout();
    detailRow->setSpacing(16);
    QVBoxLayout* metaLayout = new QVBoxLayout();
    metaLayout->addWidget(postTitleLabel);
    metaLayout->addWidget(postMetaLabel);
    metaLayout->addWidget(postStatsLabel);
    metaLayout->addStretch(1);
    detailRow->addLayout(metaLayout, 0);
    detailRow->addWidget(postContentView, 1);
    detailLayout->addLayout(detailRow);
    QLabel* commentLabel = new QLabel(tr("评论列表"), detailBox);
    detailLayout->addWidget(commentLabel);
    detailLayout->addWidget(commentList);

    QGroupBox* commentBox = new QGroupBox(tr("发表评论"), rightPanel);
    QVBoxLayout* commentLayout = new QVBoxLayout(commentBox);
    commentInput = new QTextEdit(commentBox);
    commentInput->setPlaceholderText(tr("输入评论内容..."));
    addCommentButton = new QPushButton(tr("发表评论"), commentBox);
    addCommentButton->setProperty("accent", true);
    connect(addCommentButton, &QPushButton::clicked, this, &MainWindow::handleAddComment);

    commentLayout->addWidget(commentInput);
    commentLayout->addWidget(addCommentButton);

    rightLayout->addWidget(feedBox);
    rightLayout->addWidget(detailBox);
    rightLayout->addWidget(commentBox);

    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 2);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->addWidget(splitter);

    statusBar()->showMessage(tr("系统就绪"));
    updateActionButtons();
}

void MainWindow::refreshClients() {
    QString activeName; 
    if (currentClient) {
        activeName = QString::fromStdString(currentClient->Name());
    }

    clientList->clear();
    currentClient = nullptr;

    SeqList<Client>& clients = core.getAllClients();
    for (int i = 0; i < clients.size(); ++i) {
        Client& client = clients[i];
        QString display = QString::fromStdString(client.Name() + " (" + client.ID() + ")");
        QListWidgetItem* item = new QListWidgetItem(display, clientList);
        // 使用 Name 作为 Key
        QString name = QString::fromStdString(client.Name());
        item->setData(Qt::UserRole, name);
        
        if (!activeName.isEmpty() && name == activeName) {
            clientList->setCurrentItem(item);
            currentClient = &clients[i];
        }
    }

    updateCurrentClientLabel();
    refreshFriends();
}

void MainWindow::refreshPosts() {
    QString currentKey;
    if (QListWidgetItem* item = postList->currentItem()) {
        currentKey = item->data(Qt::UserRole).toString();
    }

    postList->clear();

    SeqList<Client>& clients = core.getAllClients();
    QListWidgetItem* itemToSelect = nullptr;
    for (int i = 0; i < clients.size(); ++i) {
        Client& client = clients[i];
        for (int j = 0; j < client.posts.size(); ++j) {
            Post& post = client.posts[j];
            QString title = QString::fromStdString(post.get_title());
            QString author = QString::fromStdString(client.Name());
            QString text = QString("%1 - %2 (👍 %3, 💬 %4)")
                               .arg(author)
                               .arg(title)
                               .arg(post.likes_num())
                               .arg(post.comments_num());
            QListWidgetItem* item = new QListWidgetItem(text, postList);
            QString key = makePostKey(&client, post);
            item->setData(Qt::UserRole, key);

            if (!currentKey.isEmpty() && key == currentKey) {
                itemToSelect = item;
            }
        }
    }

    if (itemToSelect) {
        postList->setCurrentItem(itemToSelect);
    } else {
        showPostDetails(nullptr);
    }

    updateActionButtons();
}

void MainWindow::refreshFriends() {
    if (!friendList) return;
    friendList->clear();
    if (!currentClient) {
        updateActionButtons();
        return;
    }

    SeqList<Client>& allClients = core.getAllClients();
    for (int i = 0; i < allClients.size(); ++i) {
        Client* other = &allClients[i];
        if (other == currentClient) continue;

        if (core.getRelationDistance(currentClient, other) == 1) {
            QString display =
                QString::fromStdString(other->Name() + " (" + other->ID() + ")");
            QListWidgetItem* item = new QListWidgetItem(display, friendList);
            item->setData(Qt::UserRole, QString::fromStdString(other->Name()));
        }
    }
    updateActionButtons();
}

void MainWindow::updateCurrentClientLabel() {
    if (!currentUserLabel) return;
    if (currentClient) {
        currentUserLabel->setText(
            tr("当前用户：%1 (%2)")
                .arg(QString::fromStdString(currentClient->Name()))
                .arg(QString::fromStdString(currentClient->ID())));
    } else {
        currentUserLabel->setText(tr("当前用户：未选择"));
    }
}

QString MainWindow::makePostKey(Client* owner, const Post& post) const {
    if (!owner) return {};
    // Key 使用 Name
    return QString::fromStdString(owner->Name()) + ":" + QString::number(post.get_idex());
}

bool MainWindow::decodePostKey(const QString& key, QString& ownerName, int& idex) const {
    const QStringList parts = key.split(':');
    if (parts.size() != 2) return false;
    bool ok = false;
    int parsed = parts[1].toInt(&ok);
    if (!ok) return false;
    ownerName = parts[0];
    idex = parsed;
    return true;
}

Post* MainWindow::resolvePostFromItem(QListWidgetItem* item) const {
    if (!item) return nullptr;
    QString key = item->data(Qt::UserRole).toString();
    QString ownerName;
    int idex = 0;
    if (!decodePostKey(key, ownerName, idex)) return nullptr;
    return findPost(ownerName, idex);
}

Post* MainWindow::findPost(const QString& ownerName, int idex) const {
    Client* client = core.getClientByName(ownerName.toStdString());
    if (!client) return nullptr;
    for (int i = 0; i < client->posts.size(); ++i) {
        Post& post = client->posts[i];
        if (post.get_idex() == idex) {
            return &post;
        }
    }
    return nullptr;
}

void MainWindow::showPostDetails(Post* post) {
    if (!post) {
        if (postTitleLabel) postTitleLabel->setText(tr("选择一个帖子查看详情"));
        if (postMetaLabel) postMetaLabel->setText(tr("楼主信息将在此处显示"));
        if (postStatsLabel) postStatsLabel->setText(tr("👍 0  |  💬 0  | 楼层 0"));
        if (postContentView) postContentView->clear();
        commentList->clear();
        updateActionButtons();
        return;
    }

    QString author = post->author ? QString::fromStdString(post->author->Name()) : tr("未知");
    if (postTitleLabel) {
        postTitleLabel->setText(QString::fromStdString(post->get_title()));
    }
    if (postMetaLabel) {
        QString meta = tr("楼主：%1\nID：%2")
                           .arg(author)
                           .arg(post->author ? QString::fromStdString(post->author->ID()) : tr("未知"));
        postMetaLabel->setText(meta);
    }
    if (postStatsLabel) {
        QString stats =
            tr("👍 %1  |  💬 %2  |  当前楼层 %3")
                .arg(post->likes_num())
                .arg(post->comments_num())
                .arg(post->get_floor());
        postStatsLabel->setText(stats);
    }
    if (postContentView) {
        postContentView->setPlainText(QString::fromStdString(post->get_content()));
        postContentView->moveCursor(QTextCursor::Start);
    }
    refreshComments(post);
    updateActionButtons();
}

void MainWindow::refreshComments(Post* post) {
    commentList->clear();
    if (!post) return;
    for (int i = 0; i < post->comment_list.size(); ++i) {
        Comment& comment = post->comment_list[i];
        QString author =
            comment.get_author() ? QString::fromStdString(comment.get_author()->Name()) : tr("匿名");
        QString replyInfo;
        if (comment.get_comment_floor() >= 0) {
            replyInfo = tr("\n↪ 回复 %1 楼").arg(comment.get_comment_floor());
        }
        QString text = QString("#%1  %2\n%3%4")
                           .arg(comment.floor())
                           .arg(author)
                           .arg(QString::fromStdString(comment.get_content()))
                           .arg(replyInfo);
        commentList->addItem(text);
    }
}

void MainWindow::showStatusMessage(const QString& message, bool isError) {
    statusBar()->showMessage(message, isError ? 10000 : 5000);
    if (isError) {
        QMessageBox::warning(this, tr("操作提示"), message);
    }
}

void MainWindow::updateActionButtons() {
    Post* selectedPost = resolvePostFromItem(postList ? postList->currentItem() : nullptr);
    bool hasClient = currentClient != nullptr;
    bool hasPost = selectedPost != nullptr;
    if (addPostButton) addPostButton->setEnabled(hasClient);
    if (deletePostButton) {
        bool canDelete = hasClient && hasPost && selectedPost->author == currentClient;
        deletePostButton->setEnabled(canDelete);
    }
    if (likePostButton) likePostButton->setEnabled(hasClient && hasPost);
    if (analyzePostButton) analyzePostButton->setEnabled(hasPost); // 只要有帖子就能分析，不需要用户登录
    if (addCommentButton) addCommentButton->setEnabled(hasClient && hasPost);
    if (undoButton) undoButton->setEnabled(hasClient);
    if (readMessagesButton) readMessagesButton->setEnabled(hasClient);
    if (addFriendButton) addFriendButton->setEnabled(hasClient);
    if (removeFriendButton) {
        bool hasFriendSelection = hasClient && friendList && friendList->currentItem();
        removeFriendButton->setEnabled(hasFriendSelection);
    }
}

CoreStatus MainWindow::requireActiveClient() const {
    if (!currentClient) {
        QMessageBox::information(nullptr, QObject::tr("提示"), QObject::tr("请先选择一个用户。"));
        return ERR_CLIENT_NOT_FOUND;
    }
    return SUCCESS;
}

void MainWindow::onClientSelectionChanged() {
    QList<QListWidgetItem*> items = clientList->selectedItems();
    if (items.isEmpty()) {
        currentClient = nullptr;
        updateCurrentClientLabel();
        refreshFriends();
        return;
    }
    QString name = items.first()->data(Qt::UserRole).toString();
    currentClient = core.getClientByName(name.toStdString());
    updateCurrentClientLabel();
    refreshFriends();
}

void MainWindow::onPostSelectionChanged() {
    showPostDetails(resolvePostFromItem(postList->currentItem()));
}

void MainWindow::handleLoadData() {
    bool ok = core.loadData();
    refreshClients();
    refreshPosts();
    showStatusMessage(ok ? tr("成功重新加载数据。") : tr("没有可加载的数据，保持当前状态。"), !ok);
}

void MainWindow::handleSaveData() {
    bool ok = core.saveData();
    showStatusMessage(ok ? tr("数据已保存到 data/clients.json。") : tr("保存失败，请检查写入权限。"), !ok);
}

void MainWindow::handleRegisterClient() {
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("注册用户"), tr("输入唯一昵称："), QLineEdit::Normal, {}, &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    QString id = QInputDialog::getText(this, tr("注册用户"), tr("输入 ID (可选)："), QLineEdit::Normal, {}, &ok);
    if (!ok) return; 
    QString password = QInputDialog::getText(this, tr("注册用户"), tr("设置密码："), QLineEdit::Password, {}, &ok);
    if (!ok || password.trimmed().isEmpty()) return;

    CoreStatus status =
        core.registerClient(name.trimmed().toStdString(), id.trimmed().toStdString(),
                            password.trimmed().toStdString());
    if (status == SUCCESS) {
        refreshClients();
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleOpenAddPostDialog() {
    if (requireActiveClient() != SUCCESS) return;
    AddPostDialog dialog(core, this);
    dialog.setClient(currentClient);
    connect(&dialog, &AddPostDialog::postCreated, this, [this]() {
        refreshPosts();
        showStatusMessage(tr("帖子已发布。"));
    });
    dialog.exec();
}

void MainWindow::handleDeletePost() {
    if (requireActiveClient() != SUCCESS) return;
    Post* post = resolvePostFromItem(postList->currentItem());
    if (!post) {
        showStatusMessage(tr("请选择要删除的帖子。"), true);
        return;
    }
    if (post->author != currentClient) {
        showStatusMessage(tr("只能删除自己的帖子。"), true);
        return;
    }
    CoreStatus status = core.userDeletePost(currentClient, post);
    if (status == SUCCESS) {
        refreshPosts();
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleLikePost() {
    if (requireActiveClient() != SUCCESS) return;
    Post* post = resolvePostFromItem(postList->currentItem());
    if (!post) {
        showStatusMessage(tr("请选择要操作的帖子。"), true);
        return;
    }
    CoreStatus status = core.userLikePost(currentClient, post);
    if (status == SUCCESS) {
        refreshPosts();
        showPostDetails(post);
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleAnalyzePost() {
    Post* post = resolvePostFromItem(postList->currentItem());
    if (!post) {
        showStatusMessage(tr("请选择一个帖子进行分析。"), true);
        return;
    }

    std::string analysis = core.getHuffmanAnalysisResult(post);
    
    // 创建一个自定义 Dialog 显示结果
    QDialog dialog(this);
    dialog.setWindowTitle(tr("哈夫曼压缩分析报告"));
    dialog.resize(600, 500);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QTextEdit* textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(QString::fromStdString(analysis));
    textEdit->setFont(QFont("Consolas", 10)); // 使用等宽字体显示对齐
    
    QPushButton* closeBtn = new QPushButton(tr("关闭"), &dialog);
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    
    layout->addWidget(textEdit);
    layout->addWidget(closeBtn);
    
    dialog.exec();
}

void MainWindow::handleAddComment() {
    if (requireActiveClient() != SUCCESS) return;
    Post* post = resolvePostFromItem(postList->currentItem());
    if (!post) {
        showStatusMessage(tr("请选择要评论的帖子。"), true);
        return;
    }
    QString content = commentInput->toPlainText().trimmed();
    if (content.isEmpty()) {
        showStatusMessage(tr("评论内容不能为空。"), true);
        return;
    }
    CoreStatus status = core.userAddComment(currentClient, post, content.toStdString());
    if (status == SUCCESS) {
        commentInput->clear();
        refreshPosts();
        showPostDetails(post);
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleUndo() {
    if (requireActiveClient() != SUCCESS) return;
    CoreStatus status = core.userUndo(currentClient);
    if (status == SUCCESS) {
        refreshPosts();
        showPostDetails(resolvePostFromItem(postList->currentItem()));
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

QString MainWindow::describeMessage(massege* msg) const {
    if (!msg) return {};
    QString sender = msg->sender ? QString::fromStdString(msg->sender->Name()) : tr("未知用户");
    QString postTitle = msg->post ? QString::fromStdString(msg->post->get_title()) : tr("帖子");

    if (dynamic_cast<LikeMassege*>(msg)) {
        return tr("%1 赞了你的帖子「%2」").arg(sender, postTitle);
    }
    if (auto* commentMsg = dynamic_cast<CommentMassege*>(msg)) {
        QString content =
            commentMsg->comment ? QString::fromStdString(commentMsg->comment->get_content())
                                : QString();
        return tr("%1 评论了你的帖子「%2」：%3").arg(sender, postTitle, content);
    }
    return tr("收到来自 %1 的系统消息。").arg(sender);
}

void MainWindow::handleReadMessages() {
    if (requireActiveClient() != SUCCESS) return;
    LinkQueue<massege*>& queue = currentClient->getMessages();
    if (queue.empty()) {
        showStatusMessage(tr("没有新消息。"));
        return;
    }

    QStringList allMessages;
    while (!queue.empty()) {
        massege* msg = queue.dequeue();
        allMessages << describeMessage(msg);
        delete msg;
    }

    QMessageBox::information(this, tr("消息通知"), allMessages.join("\n"));
}

void MainWindow::handleRefresh() {
    refreshClients();
    refreshPosts();
    showStatusMessage(tr("列表已刷新。"));
}

void MainWindow::handleAddFriend() {
    if (requireActiveClient() != SUCCESS) return;
    SeqList<Client>& clients = core.getAllClients();
    QStringList options;
    QVector<QString> optionNames; // 保存 Name 用于查找
    
    for (int i = 0; i < clients.size(); ++i) {
        Client& candidate = clients[i];
        if (&candidate == currentClient) continue;

        // 如果已经是好友则跳过
        if (core.getRelationDistance(currentClient, &candidate) == 1) continue;
        
        QString label =
            QString::fromStdString(candidate.Name() + " (" + candidate.ID() + ")");
        options << label;
        optionNames.push_back(QString::fromStdString(candidate.Name()));
    }
    
    if (options.isEmpty()) {
        showStatusMessage(tr("没有可添加的用户。"), true);
        return;
    }
    
    bool ok = false;
    QString chosen = QInputDialog::getItem(this, tr("添加好友"), tr("选择用户："), options, 0,
                                           false, &ok);
    if (!ok || chosen.isEmpty()) return;
    
    int index = options.indexOf(chosen);
    if (index < 0 || index >= optionNames.size()) return;
    
    QString name = optionNames[index];
    Client* other = core.getClientByName(name.toStdString());
    
    CoreStatus status = core.makeFriend(currentClient, other);
    if (status == SUCCESS) {
        refreshFriends();
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleRemoveFriend() {
    if (requireActiveClient() != SUCCESS) return;
    if (!friendList || !friendList->currentItem()) {
        showStatusMessage(tr("请选择要移除的好友。"), true);
        return;
    }
    QString name = friendList->currentItem()->data(Qt::UserRole).toString();
    Client* other = core.getClientByName(name.toStdString());
    
    CoreStatus status = core.deleteFriend(currentClient, other);
    if (status == SUCCESS) {
        refreshFriends();
    }
    showStatusMessage(statusToText(status), status != SUCCESS);
}

void MainWindow::handleShowUserRanking() {
    SeqList<Client*> ranking = core.getUserInfluenceRanking();
    if (ranking.empty()) {
        showStatusMessage(tr("暂无用户数据，无法生成排行榜。"), true);
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("用户影响力排行榜"));
    dialog.resize(640, 420);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    const int rowCount = ranking.size();
    QTableWidget* table = new QTableWidget(rowCount, 5, &dialog);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setHorizontalHeaderLabels(
        {tr("排名"), tr("昵称"), tr("ID"), tr("好友数"), tr("发帖数")});

    for (int i = 0; i < rowCount; ++i) {
        Client* client = ranking[i];
        if (!client) continue;

        QString tooltip;
        SeqList<Post*> topPosts = core.getTopPostsForClient(client, 3);
        if (!topPosts.empty()) {
            QStringList lines;
            lines << tr("热门帖子：");
            for (int j = 0; j < topPosts.size(); ++j) {
                Post* post = topPosts[j];
                if (!post) continue;
                lines << tr("%1. %2 (👍 %3)")
                             .arg(j + 1)
                             .arg(QString::fromStdString(post->get_title()))
                             .arg(post->likes_num());
            }
            tooltip = lines.join("\n");
        }

        auto setItem = [&](int column, const QString& value) {
            QTableWidgetItem* item = new QTableWidgetItem(value);
            if (!tooltip.isEmpty()) {
                item->setToolTip(tooltip);
            }
            table->setItem(i, column, item);
        };

        setItem(0, QString::number(i + 1));
        setItem(1, QString::fromStdString(client->Name()));
        setItem(2, QString::fromStdString(client->ID()));
        setItem(3, QString::number(client->get_friends_num()));
        setItem(4, QString::number(client->posts.size()));
    }

    layout->addWidget(table);
    QPushButton* closeBtn = new QPushButton(tr("关闭"), &dialog);
    closeBtn->setProperty("ghost", true);
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);

    dialog.exec();
}

void MainWindow::handleShowHotPostRanking() {
    SeqList<Post*> hotPosts = core.getHotPostRanking();
    if (hotPosts.empty()) {
        showStatusMessage(tr("暂无帖子数据，无法生成排行榜。"), true);
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(tr("全局热门帖子排行榜"));
    dialog.resize(720, 420);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    const int rowCount = hotPosts.size();
    QTableWidget* table = new QTableWidget(rowCount, 5, &dialog);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setHorizontalHeaderLabels(
        {tr("排名"), tr("标题"), tr("作者"), tr("点赞数"), tr("评论数")});

    for (int i = 0; i < rowCount; ++i) {
        Post* post = hotPosts[i];
        if (!post) continue;
        QString author = post->author ? QString::fromStdString(post->author->Name()) : tr("未知");

        table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(post->get_title())));
        table->setItem(i, 2, new QTableWidgetItem(author));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(post->likes_num())));
        table->setItem(i, 4, new QTableWidgetItem(QString::number(post->comments_num())));
    }

    layout->addWidget(table);
    QPushButton* closeBtn = new QPushButton(tr("关闭"), &dialog);
    closeBtn->setProperty("ghost", true);
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);

    dialog.exec();
}

void MainWindow::handleShowRelationChain() {
    SeqList<Client>& clients = core.getAllClients();
    if (clients.size() < 2) {
        showStatusMessage(tr("用户数量不足，无法计算关系链。"), true);
        return;
    }

    QStringList options;
    for (int i = 0; i < clients.size(); ++i) {
        options << QString::fromStdString(clients[i].Name());
    }

    bool ok = false;
    QString first = QInputDialog::getItem(this, tr("选择第一个用户"),
                                          tr("用户 A："), options, 0, false, &ok);
    if (!ok || first.isEmpty()) return;

    QString second = QInputDialog::getItem(this, tr("选择第二个用户"),
                                           tr("用户 B："), options, 0, false, &ok);
    if (!ok || second.isEmpty()) return;

    if (first == second) {
        showStatusMessage(tr("请选择两个不同的用户。"), true);
        return;
    }

    Client* clientA = core.getClientByName(first.toStdString());
    Client* clientB = core.getClientByName(second.toStdString());
    if (!clientA || !clientB) {
        showStatusMessage(tr("用户不存在。"), true);
        return;
    }

    SeqList<Client*> path = core.getShortestRelationPath(clientA, clientB);
    if (path.empty()) {
        showStatusMessage(tr("这两个用户之间暂无任何关系链。"), true);
        return;
    }

    QStringList chain;
    for (int i = 0; i < path.size(); ++i) {
        Client* c = path[i];
        if (c) {
            chain << QString::fromStdString(c->Name());
        }
    }

    int distance = static_cast<int>(path.size()) - 1;
    QString message = tr("关系距离：%1\n关系链：%2")
                          .arg(distance)
                          .arg(chain.join(QStringLiteral(" → ")));
    QMessageBox::information(this, tr("最近关系链"), message);
}

void MainWindow::applyTheme() {
    const QString style = R"(
QWidget {
    background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,
                                stop:0 #f8f5ff, stop:0.5 #f0f7ff, stop:1 #fdf5f8);
    color: #1f2430;
    font-family: "Microsoft YaHei", "PingFang SC", sans-serif;
    font-size: 14px;
}
QAbstractItemView::item {
    padding: 6px 10px;
}
QAbstractItemView::item:selected {
    background: #ffe3f0;
    color: #1f2430;
}
QLabel#heroTitle {
    font-size: 22px;
    font-weight: 700;
    margin-bottom: 2px;
}
QLabel#heroSubtitle {
    color: #6b7688;
    font-size: 13px;
    margin-bottom: 10px;
}
QLabel#currentUserLabel {
    font-weight: 600;
    color: #377dff;
    margin-bottom: 8px;
}
QLabel#postTitle {
    font-size: 20px;
    font-weight: 700;
    color: #1f2430;
}
QLabel#postMeta {
    color: #6b7688;
    line-height: 1.4;
}
QLabel#postStats {
    color: #377dff;
    font-weight: 600;
    margin-top: 6px;
}
QGroupBox {
    background: #ffffff;
    border: 1px solid #dbe1ec;
    border-radius: 16px;
    margin-top: 16px;
    padding-top: 20px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 18px;
    padding: 0px 6px;
    font-weight: 600;
    color: #1f2430;
}
QListWidget {
    background: #ffffff;
    border: 1px solid #dbe1ec;
    border-radius: 12px;
    padding: 4px;
}
QListWidget::item {
    padding: 8px;
    border-radius: 8px;
}
QListWidget::item:selected {
    background: #d5e4ff;
    color: #1f2430;
}
QListWidget#commentList::item {
    background: #ffffff;
    border: 1px solid #dfe7f5;
    border-radius: 14px;
    margin: 6px;
    padding: 12px;
    color: #1f2430;
}
QListWidget#commentList::item:selected {
    background: #eef4ff;
    color: #1f2430;
    border-color: #377dff;
}
QPushButton {
    background: #e4ebf5;
    color: #1f2430;
    padding: 8px 16px;
    border-radius: 10px;
    border: none;
    font-weight: 600;
}
QPushButton:hover:!disabled {
    background: #d7e3ff;
}
QPushButton:disabled {
    background: #cfd6e5;
    color: #f9fbff;
}
QPushButton[accent="true"] {
    background: #377dff;
    color: #ffffff;
}
QPushButton[accent="true"]:hover:!disabled {
    background: #2b64d7;
}
QPushButton[danger="true"] {
    background: #ff5d73;
    color: #ffffff;
}
QPushButton[ghost="true"] {
    background: transparent;
    border: 1px solid #377dff;
    color: #377dff;
}
QTextEdit {
    border: 1px solid #dbe1ec;
    border-radius: 12px;
    background: #ffffff;
    padding: 10px;
}
QTextEdit#postContentBox {
    font-size: 16px;
    line-height: 1.6;
}
QLineEdit {
    border: 1px solid #dbe1ec;
    border-radius: 10px;
    background: #ffffff;
    padding: 6px 10px;
}
QComboBox {
    border: 1px solid #dbe1ec;
    border-radius: 10px;
    background: #ffffff;
    padding: 6px 10px;
    color: #1f2430;
}
QComboBox QAbstractItemView {
    background: #ffffff;
    color: #1f2430;
    selection-background-color: #d5e4ff;
}
QInputDialog, QDialog {
    background: #ffffff;
    color: #1f2430;
}
QStatusBar {
    background: #ffffff;
    border-top: 1px solid #dbe1ec;
}
)";
    setStyleSheet(style);
}
