#ifndef UPLOAD_DIALOG_H
#define UPLOAD_DIALOG_H

#include "User.h"
#include "PhotoManagementFacade.h"
#include "Command.h"
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>

class UploadDialog : public QDialog {
    Q_OBJECT

public:
    UploadDialog(std::shared_ptr<User> user, PhotoManagementFacade* facade,
                CommandInvoker* invoker, QWidget* parent = nullptr);

private slots:
    void browseFile();
    void handleUpload();

private:
    void setupUI();

    std::shared_ptr<User> user_;
    PhotoManagementFacade* facade_;
    CommandInvoker* invoker_;

    QLineEdit* filePathEdit_;
    QTextEdit* descriptionEdit_;
    QLineEdit* hashtagsEdit_;
    QCheckBox* resizeCheck_;
    QCheckBox* sepiaCheck_;
    QCheckBox* blurCheck_;
};

#endif // UPLOAD_DIALOG_H