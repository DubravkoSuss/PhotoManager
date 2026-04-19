#include "SearchDialog_APP.h"
#include "SearchResultsWindow_APP.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

SearchDialog::SearchDialog(PhotoManagementFacade* facade, QWidget* parent)
    : QDialog(parent), facade_(facade) {
    setupUI();
    setWindowTitle("Search Photos");
}

void SearchDialog::setupUI() {
    auto* layout = new QVBoxLayout(this);
    auto* formLayout = new QFormLayout();
    
    hashtagsEdit_ = new QLineEdit(this);
    formLayout->addRow("Hashtags:", hashtagsEdit_);
    
    authorEdit_ = new QLineEdit(this);
    formLayout->addRow("Author:", authorEdit_);
    
    layout->addLayout(formLayout);
    
    searchButton_ = new QPushButton("Search", this);
    connect(searchButton_, &QPushButton::clicked, this, &SearchDialog::performSearch);
    layout->addWidget(searchButton_);
}

void SearchDialog::performSearch() {
    PhotoSearchCriteria criteria;
    
    if (!hashtagsEdit_->text().isEmpty()) {
        criteria.setHashtags(hashtagsEdit_->text().split(',', Qt::SkipEmptyParts));
    }
    if (!authorEdit_->text().isEmpty()) {
        criteria.setAuthor(authorEdit_->text());
    }
    
    auto results = facade_->SearchPhotos(criteria);

    
    // results in new window
    auto* resultsWindow = new SearchResultsWindow(results);
    resultsWindow->setAttribute(Qt::WA_DeleteOnClose); // Delete window when closed
    resultsWindow->show();
    
    // Close search dialog
    accept();
}