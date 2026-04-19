#include "SearchResultsWindow_APP.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPainter>
#include "FileSizeFormatter_APP.h"

SearchResultsWindow::SearchResultsWindow(QList<std::shared_ptr<Photo>> photos, QWidget *parent)
    : QWidget(parent, Qt::Window)
    , photos_(photos)
{
    setupUI();
    setWindowTitle(QString("Search Results - %1 found").arg(photos.size()));
    resize(1000, 700);
}

void SearchResultsWindow::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    
    // Title
    auto* titleLabel = new QLabel(QString("Found %1 photos").arg(photos_.size()));
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(titleLabel);

    // Scrollable area for photo grid
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none; background-color: #f0f0f0;");

    auto* photoGridWidget = new QWidget();
    photoGridLayout_ = new QGridLayout(photoGridWidget);
    photoGridLayout_->setSpacing(20);
    photoGridLayout_->setContentsMargins(30, 30, 30, 30);

    // Populate grid
    for (const auto& photo : photos_) {
        addPhotoThumbnail(photo);
    }
    
    // Add spacer to push content up if few items
    if (photos_.isEmpty()) {
        auto* emptyLabel = new QLabel("No photos matches your criteria.");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; font-size: 16px;");
        photoGridLayout_->addWidget(emptyLabel, 0, 0);
    } else {
        photoGridLayout_->setRowStretch(photoGridLayout_->rowCount(), 1);
    }

    scrollArea->setWidget(photoGridWidget);
    mainLayout->addWidget(scrollArea);
}

void SearchResultsWindow::addPhotoThumbnail(std::shared_ptr<Photo> photo) {
    auto* photoCard = new QWidget();
    photoCard->setStyleSheet(
        "QWidget { "
        "  background: #ffffff; "
        "  border: 1px solid #e0e0e0; "
        "  border-radius: 12px; "
        "}"
        "QLabel { color: #333333; }"
    );
    
    auto* cardLayout = new QVBoxLayout(photoCard);
    cardLayout->setContentsMargins(10, 10, 10, 15);
    cardLayout->setSpacing(8);

    // Load and display image
    QLabel* imageLabel = new QLabel();
    imageLabel->setFixedSize(200, 200);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("background-color: #f5f5f5; border-radius: 8px; border: 1px solid #eeeeee;");
    
    QImage image(photo->getStoragePath());
    if (image.isNull()) {
        // Create placeholder
        QPixmap placeholder(200, 200);
        placeholder.fill(QColor("#f5f5f5"));
        QPainter painter(&placeholder);
        painter.setPen(QColor("#888888"));
        painter.setFont(QFont("Arial", 40));
        painter.drawText(placeholder.rect(), Qt::AlignCenter, "📷");
        imageLabel->setPixmap(placeholder);
    } else {
        QImage thumbnail = image.scaled(200, 200, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        
        // Center crop
        QImage squared = thumbnail.copy(
            (thumbnail.width() - 200)/2, 
            (thumbnail.height() - 200)/2, 
            200, 200);
            
        imageLabel->setPixmap(QPixmap::fromImage(squared));
    }
    
    cardLayout->addWidget(imageLabel);

    // Display filename
    auto* filenameLabel = new QLabel(photo->getFilename());
    filenameLabel->setStyleSheet("font-weight: bold; font-size: 13px; border: none; background: transparent; color: #222;");
    filenameLabel->setWordWrap(true);
    filenameLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(filenameLabel);

    // Display author
    auto* authorLabel = new QLabel("By: " + photo->getAuthorName());
    authorLabel->setStyleSheet("color: #444; font-size: 11px; border: none; background: transparent;");
    authorLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(authorLabel);

    // Display date
    auto* dateLabel = new QLabel(photo->getUploadDateTime().toString("yyyy-MM-dd HH:mm"));
    dateLabel->setStyleSheet("color: #666; font-size: 10px; border: none; background: transparent;");
    dateLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(dateLabel);

    // Display file size
    auto* sizeLabel = new QLabel(FileSizeFormatter::format(photo->getFileSize()));
    sizeLabel->setStyleSheet("color: #888; font-size: 10px; border: none; background: transparent;");
    sizeLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(sizeLabel);

    // Add to grid layout (4 columns)
    int currentRow = photoGridLayout_->count() / 4;
    int currentCol = photoGridLayout_->count() % 4;
    photoGridLayout_->addWidget(photoCard, currentRow, currentCol);
}

