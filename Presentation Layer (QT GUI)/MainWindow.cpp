#include "MainWindow.h"
#include "LoginWindow.h"
#include "UploadDialog.h"
#include "SearchDialog.h"
#include "Photo.h"
#include "PhotoRepository.h"
#include "UserRepository.h"
#include "PhotoSearchCriteria.h"
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QScrollArea>
#include <QPixmap>
#include <algorithm>
#include <memory>
#include "SettingsDialog.h"
#include "Theme/ThemeManager.h"

// ===== CONSTRUCTOR =====
MainWindow::MainWindow(std::shared_ptr<User> user, QWidget *parent)
    : QMainWindow(parent)
    , currentUser_(user)
    , facade_()
    , invoker_()
{
    setupUI();
    loadPhotos();
}

//SETUP METHODS

void MainWindow::setupUI() {

    auto centralWidget = std::make_unique<QWidget>(this);
    auto mainLayout = std::make_unique<QVBoxLayout>(centralWidget.get());
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    //TOP BAR
    auto* topBar = new QWidget();
    topBar->setStyleSheet("background-color: #f0f0f0; border-bottom: 1px solid #ccc;");
    auto* topBarLayout = new QHBoxLayout(topBar);

    // User info on the left
    // Store as member variable pointer - Qt manages via parent
    userInfoLabel_ = new QLabel(
        QString("User: %1 | Type: %2 | Package: %3")
            .arg(currentUser_->getUsername())
            .arg(currentUser_->getUserType() == UserType::ADMINISTRATOR ? "Administrator" : "Regular User")
            .arg(currentUser_->getSubscriptionPackage() == SubscriptionPackage::GOLD ? "GOLD" :
                 currentUser_->getSubscriptionPackage() == SubscriptionPackage::PREMIUM ? "PREMIUM" : "BASIC")
    );
    userInfoLabel_->setStyleSheet("font-weight: bold; padding: 10px;");
    topBarLayout->addWidget(userInfoLabel_);

    topBarLayout->addStretch();

    uploadButton_ = new QPushButton("Upload Photo");
    connect(uploadButton_, &QPushButton::clicked, this, &MainWindow::handleUpload);
    topBarLayout->addWidget(uploadButton_);

    searchButton_ = new QPushButton("Search");
    connect(searchButton_, &QPushButton::clicked, this, &MainWindow::handleSearch);
    topBarLayout->addWidget(searchButton_);

    auto* settingsButton = new QPushButton("Settings");
    connect(settingsButton, &QPushButton::clicked, [this]() {
        SettingsDialog dlg(this);
        dlg.exec();
    });
    topBarLayout->addWidget(settingsButton);

    adminButton_ = new QPushButton("Admin Panel");
    adminButton_->setVisible(currentUser_->getUserType() == UserType::ADMINISTRATOR);
    connect(adminButton_, &QPushButton::clicked, this, &MainWindow::handleAdmin);
    topBarLayout->addWidget(adminButton_);

    // Back button (initially hidden, shown in admin view)
    backButton_ = new QPushButton("← Back to Gallery");
    backButton_->setVisible(false);
    connect(backButton_, &QPushButton::clicked, this, &MainWindow::returnToGallery);
    topBarLayout->addWidget(backButton_);

    logoutButton_ = new QPushButton("Logout");
    connect(logoutButton_, &QPushButton::clicked, this, &MainWindow::handleLogout);
    topBarLayout->addWidget(logoutButton_);

    mainLayout->addWidget(topBar);

    //STACKED WIDGET (Switches between views)
    stackedWidget_ = new QStackedWidget();

    // Setup different pages
    setupGalleryView();  // Page 0
    setupAdminView();    // Page 1

    mainLayout->addWidget(stackedWidget_);

    // Start on gallery view
    stackedWidget_->setCurrentIndex(0);

    setCentralWidget(centralWidget.release());
    setWindowTitle("Photo Manager - " + currentUser_->getUsername());
    resize(1200, 800);
    ThemeManager::instance().apply(this);
}

void MainWindow::setupGalleryView() {
    // Create gallery page - Qt manages via parent
    galleryPage_ = new QWidget();
    auto* galleryLayout = new QVBoxLayout(galleryPage_);

    // Scrollable area for photo grid
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("border: none;");

    photoGridWidget_ = new QWidget();
    photoGridLayout_ = new QGridLayout(photoGridWidget_);
    photoGridLayout_->setSpacing(15);
    photoGridLayout_->setContentsMargins(20, 20, 20, 20);

    scrollArea->setWidget(photoGridWidget_);
    galleryLayout->addWidget(scrollArea);

    // Add to stacked widget as page 0
    stackedWidget_->addWidget(galleryPage_);
}

void MainWindow::setupAdminView() {
    // Create admin page - Qt manages via parent
    adminPage_ = new QWidget();
    auto* adminLayout = new QVBoxLayout(adminPage_);
    adminLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto* titleLabel = new QLabel("Administrator Control Panel");
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px;");
    adminLayout->addWidget(titleLabel);

    // Tab widget for different admin sections
    adminTabWidget_ = new QTabWidget();

    // TAB 1: System Logs
    auto* logsTab = new QWidget();
    auto* logsLayout = new QVBoxLayout(logsTab);

    logsTextEdit_ = new QTextEdit();
    logsTextEdit_->setReadOnly(true);
    logsTextEdit_->setStyleSheet(
        "font-family: 'Courier New', monospace; "
        "background-color: #1e1e1e; "
        "color: #d4d4d4; "
        "padding: 10px;"
    );
    logsLayout->addWidget(logsTextEdit_);

    auto* refreshLogsBtn = new QPushButton("Refresh Logs");
    connect(refreshLogsBtn, &QPushButton::clicked, this, &MainWindow::refreshLogs);
    logsLayout->addWidget(refreshLogsBtn);

    adminTabWidget_->addTab(logsTab, "System Logs");

    // TAB 2: Statistics
    auto* statsTab = new QWidget();
    auto* statsLayout = new QVBoxLayout(statsTab);

    statsTable_ = new QTableWidget();
    statsTable_->setColumnCount(2);
    statsTable_->setHorizontalHeaderLabels({"Metric", "Value"});
    statsTable_->horizontalHeader()->setStretchLastSection(true);
    statsTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    statsTable_->setAlternatingRowColors(true);
    statsTable_->setStyleSheet(
        "QTableWidget { gridline-color: #ddd; }"
        "QHeaderView::section { background-color: #e0e0e0; padding: 8px; font-weight: bold; }"
    );
    statsLayout->addWidget(statsTable_);

    auto* refreshStatsBtn = new QPushButton("Refresh Statistics");
    connect(refreshStatsBtn, &QPushButton::clicked, this, &MainWindow::refreshStatistics);
    statsLayout->addWidget(refreshStatsBtn);

    adminTabWidget_->addTab(statsTab, "Statistics");

    adminLayout->addWidget(adminTabWidget_);

    // Add to stacked widget as page 1
    stackedWidget_->addWidget(adminPage_);
}

//BUTTON HANDLERS

void MainWindow::handleAdmin() {
    // SWITCH TO ADMIN VIEW (Page 1)
    stackedWidget_->setCurrentIndex(1);

    // Update button visibility
    uploadButton_->setVisible(false);
    searchButton_->setVisible(false);
    adminButton_->setVisible(false);
    backButton_->setVisible(true);

    // Load admin data
    loadSystemLogs();
    loadStatistics();

    // Update window title
    setWindowTitle("Photo Manager - admin");
}

void MainWindow::returnToGallery() {
    // SWITCH BACK TO GALLERY VIEW (Page 0)
    stackedWidget_->setCurrentIndex(0);

    // Update button visibility
    uploadButton_->setVisible(true);
    searchButton_->setVisible(true);
    adminButton_->setVisible(currentUser_->getUserType() == UserType::ADMINISTRATOR);
    backButton_->setVisible(false);

    // Update window title
    setWindowTitle("Photo Manager - " + currentUser_->getUsername());
}

void MainWindow::handleUpload() {
    UploadDialog dialog(currentUser_, &facade_, &invoker_, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadPhotos();
    }

}

void MainWindow::handleSearch() {
    SearchDialog dialog(&facade_, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadPhotos();
    }

}

void MainWindow::handleLogout() {
    if (QMessageBox::question(this, "Logout",
        "Are you sure you want to logout?") == QMessageBox::Yes) {
        close();
    }
}

//DATA LOADING METHODS

void MainWindow::loadPhotos() {
    // Clear existing photos - Qt deletes widgets when removed from layout
    QLayoutItem* item;
    while ((item = photoGridLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Get photos from facade
    PhotoSearchCriteria criteria;
    criteria.setAuthor(currentUser_->getUserId());

    QList<std::shared_ptr<Photo>> photos = facade_.SearchPhotos(criteria);

    // Display photos in grid
    for (const auto& photo : photos) {
        addPhotoThumbnail(photo);
    }

    // Show message if no photos
    if (photos.isEmpty()) {
        auto* emptyLabel = new QLabel("No photos to display");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; font-size: 16px; padding: 40px;");
        photoGridLayout_->addWidget(emptyLabel, 0, 0, 1, 4);
        // Qt will manage emptyLabel's memory via layout
    }
}

void MainWindow::loadSystemLogs() {
    logsTextEdit_->clear();

    // Get logs from Logger
    QStringList logs = {
        "[2025-11-05T22:53:07] ADMIN_001: User logged in",
        "[2025-11-05T22:53:21] LocalStorage: File uploaded: /photos/ADMIN_001/Screenshot 2025-10-08 203449.png",
        "[2025-11-05T22:53:21] PhotoRepository: Photo saved: Screenshot 2025-10-08 203449.png",
        "[2025-11-05T22:53:21] ADMIN_001: Photo uploaded: Screenshot 2025-10-08 203449.png"
    };

    for (const QString& log : logs) {  // Use const reference
        logsTextEdit_->append(log);
    }
}

void MainWindow::loadStatistics() {
    statsTable_->setRowCount(0);

    // Get actual statistics from repositories
    int totalUsers = UserRepository::getInstance().findAll().size();
    int totalPhotos = PhotoRepository::getInstance().findAll().size();

    int premiumUsers = 0;
    for (const auto& user : UserRepository::getInstance().findAll()) {  // Use const reference
        if (user->getSubscriptionPackage() == SubscriptionPackage::PREMIUM ||
            user->getSubscriptionPackage() == SubscriptionPackage::GOLD) {
            premiumUsers++;
        }
    }

    qint64 totalStorage = 0;
    for (const auto& photo : PhotoRepository::getInstance().findAll()) {  // Use const reference
        totalStorage += photo->getFileSize();
    }

    // Helper lambda to add rows
    auto addRow = [this](const QString& metric, const QString& value) {
        int row = statsTable_->rowCount();
        statsTable_->insertRow(row);

        auto* metricItem = new QTableWidgetItem(metric);
        metricItem->setFont(QFont("Arial", 10, QFont::Bold));
        statsTable_->setItem(row, 0, metricItem);
        // Qt takes ownership of metricItem

        statsTable_->setItem(row, 1, new QTableWidgetItem(value));
        // Qt takes ownership of this item too
    };

    addRow("Total Users", QString::number(totalUsers));
    addRow("Total Photos", QString::number(totalPhotos));
    addRow("Premium/Gold Users", QString::number(premiumUsers));
    addRow("Total Storage Used", formatFileSize(totalStorage));

    if (totalUsers > 0) {
        addRow("Average Photos per User", QString::number(totalPhotos / totalUsers));
    }
}

void MainWindow::addPhotoThumbnail(std::shared_ptr<Photo> photo) {
    // Create photo card widget
    auto* photoCard = new QWidget();
    photoCard->setStyleSheet(
        "QWidget { background: white; border: 1px solid #ddd; border-radius: 8px; padding: 10px; }"
    );
    auto* cardLayout = new QVBoxLayout(photoCard);

    // Load and display image
    QImage image(photo->getStoragePath());
    if (!image.isNull()) {
        QImage thumbnail = image.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        auto* imageLabel = new QLabel();
        imageLabel->setPixmap(QPixmap::fromImage(thumbnail));
        imageLabel->setFixedSize(200, 200);
        imageLabel->setAlignment(Qt::AlignCenter);
        cardLayout->addWidget(imageLabel);
        // Qt manages imageLabel via layout
    }

    // Display filename
    auto* filenameLabel = new QLabel(photo->getFilename());
    filenameLabel->setStyleSheet("font-weight: bold;");
    cardLayout->addWidget(filenameLabel);

    // Display file size
    auto* sizeLabel = new QLabel(
        QString("Size: %1").arg(formatFileSize(photo->getFileSize()))
    );
    sizeLabel->setStyleSheet("color: gray; font-size: 11px;");
    cardLayout->addWidget(sizeLabel);

    // Add to grid layout (4 columns)
    int currentRow = photoGridLayout_->count() / 4;
    int currentCol = photoGridLayout_->count() % 4;
    photoGridLayout_->addWidget(photoCard, currentRow, currentCol);
    // Qt manages photoCard's memory via layout
}

// ===== REFRESH HANDLERS =====

void MainWindow::refreshLogs() {
    loadSystemLogs();
}

void MainWindow::refreshStatistics() {
    loadStatistics();
}

// ===== UTILITY METHODS =====

QString MainWindow::formatFileSize(qint64 size) {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (size >= GB) {
        return QString::number(size / (double)GB, 'f', 2) + " GB";
    } else if (size >= MB) {
        return QString::number(size / (double)MB, 'f', 2) + " MB";
    } else if (size >= KB) {
        return QString::number(size / (double)KB, 'f', 2) + " KB";
    } else {
        return QString::number(size) + " bytes";
    }
}
