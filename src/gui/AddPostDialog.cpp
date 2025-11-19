#include "AddPostDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

QString statusToText(CoreStatus status) {
    switch (status) {
    case SUCCESS: return QObject::tr("操作成功");
    case ERR_CLIENT_EXISTS: return QObject::tr("用户已存在");
    case ERR_LIST_FULL: return QObject::tr("用户列表已满");
    case ERR_CLIENT_NOT_FOUND: return QObject::tr("未找到用户");
    case ERR_POST_NOT_FOUND: return QObject::tr("未找到帖子");
    case ERR_COMMENT_NOT_FOUND: return QObject::tr("未找到评论");
    case ERR_NO_ACTION_TO_UNDO: return QObject::tr("没有可撤销的操作");
    case ERR_ACTION_INVALID: return QObject::tr("操作已失效");
    case ERR_ALREADY_FRIENDS: return QObject::tr("已经是好友了");
    case ERR_NOT_FRIENDS: return QObject::tr("尚未建立好友关系");
    case ERR_SELF_RELATION: return QObject::tr("不能与自己建立关系");
    default: return QObject::tr("未知错误");
    }
}

} // namespace

AddPostDialog::AddPostDialog(Core& coreRef, QWidget* parent)
    : QDialog(parent)
    , core(coreRef) {
    setWindowTitle(tr("发布新帖子"));
    setModal(true);
    resize(420, 320);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    const QString dialogStyle = R"(
QDialog {
    background-color: #f7f9fc;
    color: #1f2430;
    font-family: "Microsoft YaHei", "PingFang SC", sans-serif;
    font-size: 14px;
}
QLabel#dialogTitle {
    font-size: 20px;
    font-weight: 700;
}
QLabel#dialogSubtitle {
    color: #6b7688;
    margin-bottom: 8px;
}
QLabel#dialogClient {
    font-weight: 600;
    color: #377dff;
}
QLineEdit, QTextEdit {
    border: 1px solid #dbe1ec;
    border-radius: 10px;
    background: #ffffff;
    padding: 8px;
}
QPushButton {
    background: #e4ebf5;
    color: #1f2430;
    border-radius: 10px;
    padding: 8px 16px;
    font-weight: 600;
    border: none;
}
QPushButton[accent="true"] {
    background: #377dff;
    color: #ffffff;
}
QPushButton:disabled {
    background: #cfd6e5;
}
)";
    setStyleSheet(dialogStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel(tr("创建全新帖子"), this);
    titleLabel->setObjectName("dialogTitle");
    QLabel* subtitleLabel = new QLabel(tr("为内容编写标题和正文，立即发布"), this);
    subtitleLabel->setObjectName("dialogSubtitle");

    clientLabel = new QLabel(tr("当前用户：未选择"), this);
    clientLabel->setObjectName("dialogClient");

    titleEdit = new QLineEdit(this);
    titleEdit->setPlaceholderText(tr("请输入标题"));

    contentEdit = new QTextEdit(this);
    contentEdit->setPlaceholderText(tr("请输入正文..."));

    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow(tr("标题"), titleEdit);
    formLayout->addRow(tr("正文"), contentEdit);

    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal, this);
    submitButton = buttonBox->addButton(tr("发布"), QDialogButtonBox::AcceptRole);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddPostDialog::handleSubmit);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &AddPostDialog::reject);
    submitButton->setProperty("accent", true);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(clientLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    updateClientLabel();
    submitButton->setEnabled(false);
}

void AddPostDialog::setClient(Client* c) {
    client = c;
    updateClientLabel();
    submitButton->setEnabled(client != nullptr);
}

void AddPostDialog::updateClientLabel() {
    if (!clientLabel) return;
    if (client) {
        clientLabel->setText(
            tr("当前用户：%1 (%2)")
                .arg(QString::fromStdString(client->Name()))
                .arg(QString::fromStdString(client->ID())));
    } else {
        clientLabel->setText(tr("当前用户：未选择"));
    }
}

void AddPostDialog::handleSubmit() {
    if (!client) {
        QMessageBox::information(this, tr("提示"), tr("请先选择用户。"));
        return;
    }
    QString title = titleEdit->text().trimmed();
    QString content = contentEdit->toPlainText().trimmed();
    if (title.isEmpty() || content.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("标题和正文均不能为空。"));
        return;
    }

    CoreStatus status = core.userAddPost(client, title.toStdString(), content.toStdString());
    if (status == SUCCESS) {
        emit postCreated();
        accept();
    } else {
        QMessageBox::warning(this, tr("提交失败"), tr("失败原因：%1").arg(statusToText(status)));
    }
}
