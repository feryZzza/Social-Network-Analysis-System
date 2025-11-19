#ifndef GUI_ADDPOSTDIALOG_H
#define GUI_ADDPOSTDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;

#include "manager/core.h"

class AddPostDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddPostDialog(Core& coreRef, QWidget* parent = nullptr);
    ~AddPostDialog() override = default;

    void setClient(Client* client);

signals:
    void postCreated();

private slots:
    void handleSubmit();

private:
    void updateClientLabel();

    Core& core;
    Client* client = nullptr;

    QLabel* clientLabel = nullptr;
    QLineEdit* titleEdit = nullptr;
    QTextEdit* contentEdit = nullptr;
    QPushButton* submitButton = nullptr;
};

#endif // GUI_ADDPOSTDIALOG_H