#include "PhotoDetailsDialog_APP.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QPixmap>
#include <QFileDialog>

PhotoDetailsDialog::PhotoDetailsDialog(std::shared_ptr<Photo> photo, 
                                     std::shared_ptr<User> currentUser,
                                     PhotoManagementFacade* facade,
                                     QWidget* parent)
    : QDialog(parent)
    , photo_(photo)
    , currentUser_(currentUser)
    , facade_(facade)
{
    setupUI();
    loadData();
    setWindowTitle("Photo Details - " + photo->getFilename());
    resize(500, 600);
}

void PhotoDetailsDialog::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);

    // Image Preview
    imageLabel_ = new QLabel();
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setFixedSize(450, 300);
    imageLabel_->setStyleSheet("background-color: #eee; border: 1px solid #ccc;");
    imageLabel_->setScaledContents(true);
    mainLayout->addWidget(imageLabel_);

    // Info Label (Filename, Date, etc)
    infoLabel_ = new QLabel();
    infoLabel_->setStyleSheet("color: #666; font-size: 11px; margin-bottom: 10px;");
    mainLayout->addWidget(infoLabel_);

    // Form for editing
    auto* formLayout = new QFormLayout();

    descriptionEdit_ = new QTextEdit();
    descriptionEdit_->setMaximumHeight(80);
    formLayout->addRow("Description:", descriptionEdit_);

    hashtagsEdit_ = new QLineEdit();
    formLayout->addRow("Hashtags:", hashtagsEdit_);

    mainLayout->addLayout(formLayout);

    // Buttons
    auto* btnLayout = new QHBoxLayout();
    
    auto* saveBtn = new QPushButton("Save Changes");
    connect(saveBtn, &QPushButton::clicked, this, &PhotoDetailsDialog::onSaveClicked);
    btnLayout->addWidget(saveBtn);

    // UNDO BUTTON (Memento Pattern)
    auto* undoBtn = new QPushButton("Undo Last Edit");
    connect(undoBtn, &QPushButton::clicked, this, &PhotoDetailsDialog::onUndoClicked);
    btnLayout->addWidget(undoBtn);

    // DOWNLOAD
    auto* downloadBtn = new QPushButton("Download");
    connect(downloadBtn, &QPushButton::clicked, [this](){
        QString savePath = QFileDialog::getSaveFileName(this, "Save Photo", photo_->getFilename());
        if(!savePath.isEmpty()) {
            facade_->downloadPhoto(photo_->getPhotoId(), savePath);
            QMessageBox::information(this, "Success", "Photo saved.");
        }
    });
    btnLayout->addWidget(downloadBtn);

    auto* closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);

    mainLayout->addLayout(btnLayout);
}

void PhotoDetailsDialog::loadData() {
    // Load Image
    QPixmap pixmap(photo_->getStoragePath());
    if (!pixmap.isNull()) {
        imageLabel_->setPixmap(pixmap.scaled(imageLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imageLabel_->setText("Image not found");
    }

    // Load Info
    infoLabel_->setText(QString("File: %1\nUploaded: %2\nSize: %3 bytes")
        .arg(photo_->getFilename())
        .arg(photo_->getUploadDateTime().toString())
        .arg(photo_->getFileSize()));

    // Load Editable Data
    descriptionEdit_->setText(photo_->getDescription());
    hashtagsEdit_->setText(photo_->getHashtags().join(", "));
}

void PhotoDetailsDialog::onSaveClicked() {
    QString newDesc = descriptionEdit_->toPlainText();
    QStringList newTags = hashtagsEdit_->text().split(',', Qt::SkipEmptyParts);
    
    // Trim tags
    for(auto& tag : newTags) tag = tag.trimmed();

    facade_->updatePhoto(currentUser_, photo_->getPhotoId(), newDesc, newTags);
    
    QMessageBox::information(this, "Success", "Photo details updated.");
}

void PhotoDetailsDialog::onUndoClicked() {
    facade_->undoLastEdit(currentUser_, photo_->getPhotoId());

    loadData();
    
    QMessageBox::information(this, "Undone", "Last edit undone.");
}