#ifndef SEARCH_DIALOG_H
#define SEARCH_DIALOG_H

#include "PhotoManagementFacade.h"
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    SearchDialog(PhotoManagementFacade* facade, QWidget* parent = nullptr);

private slots:
    void performSearch();

private:
    void setupUI();
    PhotoManagementFacade* facade_;
    QLineEdit* hashtagsEdit_;
    QLineEdit* authorEdit_;
    QPushButton* searchButton_;
};

#endif // SEARCH_DIALOG_H