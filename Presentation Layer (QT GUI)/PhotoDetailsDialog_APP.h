#ifndef PHOTO_DETAILS_DIALOG_H
#define PHOTO_DETAILS_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <memory>
#include "Photo.h"
#include "User.h"
#include "PhotoManagementFacade_APP.h"

class PhotoDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PhotoDetailsDialog(std::shared_ptr<Photo> photo, 
                              std::shared_ptr<User> currentUser,
                              PhotoManagementFacade* facade,
                              QWidget* parent = nullptr);

private slots:
    void onSaveClicked();
    void onUndoClicked();

private:
    void setupUI();
    void loadData();

    std::shared_ptr<Photo> photo_;
    std::shared_ptr<User> currentUser_;
    PhotoManagementFacade* facade_;

    QLabel* imageLabel_;
    QLabel* infoLabel_;
    QTextEdit* descriptionEdit_;
    QLineEdit* hashtagsEdit_;
};

#endif // PHOTO_DETAILS_DIALOG_H