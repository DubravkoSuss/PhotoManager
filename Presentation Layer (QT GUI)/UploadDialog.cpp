#include "UploadDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>

UploadDialog::UploadDialog(std::shared_ptr<User> user, PhotoManagementFacade* facade,
                          CommandInvoker* invoker, QWidget* parent)
    : QDialog(parent), user_(user), facade_(facade), invoker_(invoker) {
    setupUI();
    setWindowTitle("Upload Photo");
    resize(500, 400);
}

void UploadDialog::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    
    auto* formLayout = new QFormLayout();
    
    // File selection
    auto* fileLayout = new QHBoxLayout();
    filePathEdit_ = new QLineEdit(this);
    fileLayout->addWidget(filePathEdit_);
    auto* browseBtn = new QPushButton("Browse", this);
    connect(browseBtn, &QPushButton::clicked, this, &UploadDialog::browseFile);
    fileLayout->addWidget(browseBtn);
    formLayout->addRow("File:", fileLayout);
    
    // Description
    descriptionEdit_ = new QTextEdit(this);
    descriptionEdit_->setMaximumHeight(80);
    formLayout->addRow("Description:", descriptionEdit_);
    
    // Hashtags
    hashtagsEdit_ = new QLineEdit(this);
    hashtagsEdit_->setPlaceholderText("Comma-separated tags");
    formLayout->addRow("Hashtags:", hashtagsEdit_);
    
    mainLayout->addLayout(formLayout);
    
    // Processing options
    auto* optionsGroup = new QGroupBox("Processing Options", this);
    auto* optionsLayout = new QVBoxLayout(optionsGroup);
    
    resizeCheck_ = new QCheckBox("Resize (800x600)", this);
    optionsLayout->addWidget(resizeCheck_);
    
    sepiaCheck_ = new QCheckBox("Apply Sepia Filter", this);
    optionsLayout->addWidget(sepiaCheck_);
    
    blurCheck_ = new QCheckBox("Apply Blur", this);
    optionsLayout->addWidget(blurCheck_);
    
    mainLayout->addWidget(optionsGroup);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    auto* uploadBtn = new QPushButton("Upload", this);
    connect(uploadBtn, &QPushButton::clicked, this, &UploadDialog::handleUpload);
    buttonLayout->addWidget(uploadBtn);
    
    auto* cancelBtn = new QPushButton("Cancel", this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelBtn);
    
    mainLayout->addLayout(buttonLayout);
}

void UploadDialog::browseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Image",
        QDir::homePath(), "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!fileName.isEmpty()) {
        filePathEdit_->setText(fileName);
    }
}

void UploadDialog::handleUpload() {
    QString filePath = filePathEdit_->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file");
        return;
    }
    
    QString description = descriptionEdit_->toPlainText();
    QStringList hashtags = hashtagsEdit_->text().split(',', Qt::SkipEmptyParts);
    for (QString& tag : hashtags) {
        tag = tag.trimmed();
    }
    
    // Build processor chain using Decorator pattern
    std::shared_ptr<ImageProcessor> processor = std::make_shared<BaseImageProcessor>();
    if (resizeCheck_->isChecked()) {
        processor = std::make_shared<ResizeDecorator>(processor, 800, 600);
    }
    if (sepiaCheck_->isChecked()) {
        processor = std::make_shared<SepiaDecorator>(processor);
    }
    if (blurCheck_->isChecked()) {
        processor = std::make_shared<BlurDecorator>(processor);
    }
    
    // Use Command pattern
    auto uploadCommand = std::make_shared<UploadPhotoCommand>(
        facade_, user_, filePath, description, hashtags, processor);
    invoker_->executeCommand(uploadCommand);
    
    QMessageBox::information(this, "Success", "Photo uploaded successfully!");
    accept();
}