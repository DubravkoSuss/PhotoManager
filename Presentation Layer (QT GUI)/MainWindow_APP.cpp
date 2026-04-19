#include "MainWindow_APP.h"
#include "LoginWindow_APP.h"
#include "UploadDialog_APP.h"
#include "SearchDialog_APP.h"
#include "SearchResultsWindow_APP.h"
#include "PhotoDetailsDialog_APP.h"
#include "Photo.h"
#include "PhotoRepository_APP.h"
#include "UserRepository_APP.h"
#include "PhotoSearchCriteria_APP.h"
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QMessageBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QScrollArea>
#include <QPixmap>
#include <algorithm>
#include <memory>
#include "SettingsDialog.h"
#include "Theme/ThemeManager.h"
#include "MetricsCollector_APP.h"
#include "FileSizeFormatter_APP.h"
#include <QPainter>
#include <numeric>
#include <QApplication>

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


    PackageLimits limits = User::getPackageLimits(currentUser_->getSubscriptionPackage());
    QString limitStr = (limits.dailyUploadLimit == -1) ? "Unlimited" : QString::number(limits.dailyUploadLimit);

    userInfoLabel_ = new QLabel(
        QString("User: %1 | Package: %2 | Usage: %3/%4")
            .arg(currentUser_->getUsername())
            .arg(User::subscriptionPackageToString(currentUser_->getSubscriptionPackage()))
            .arg(currentUser_->getUploadCount())
            .arg(limitStr)
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

    mainLayout->addWidget(topBar);
    
    // Add explicit spacing to separate top bar from content
    mainLayout->addSpacing(40);

    //STACKED WIDGET (Switches between views)
    stackedWidget_ = new QStackedWidget();

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
    photoGridLayout_->setSpacing(20);
    photoGridLayout_->setContentsMargins(30, 40, 30, 30);

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

    // GLOBAL GALLERY TOGGLE
    auto* globalCheck = new QCheckBox("View Global Gallery (All Users)");
    globalCheck->setFont(QFont("Arial", 10));
    connect(globalCheck, &QCheckBox::toggled, [this](bool checked) {
        viewGlobalGallery_ = checked;

    });
    adminLayout->addWidget(globalCheck);

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



//DATA LOADING METHODS

void MainWindow::loadPhotos() {
    // Clear existing photos - Qt
    QLayoutItem* item;
    while ((item = photoGridLayout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Get photos from facade
    PhotoSearchCriteria criteria;

    if (!viewGlobalGallery_ || currentUser_->getUserType() != UserType::ADMINISTRATOR) {
        criteria.setAuthor(currentUser_->getUsername());
    }

    QList<std::shared_ptr<Photo>> photos = facade_.SearchPhotos(criteria);

    QStringList photoIds;
    photoIds.reserve(photos.size());
    std::transform(photos.cbegin(), photos.cend(), std::back_inserter(photoIds),
                   [](const std::shared_ptr<Photo>& photo) {
                       return photo->getPhotoId();
                   });

    // Display photos in grid
    for (const auto& photo : photos) {
        addPhotoThumbnail(photo);
    }

    // Show message if no photos
    if (photos.isEmpty()) {
        auto* emptyLabel = new QLabel(
            (viewGlobalGallery_ && currentUser_->getUserType() == UserType::ADMINISTRATOR) 
            ? "No photos in system." 
            : "No photos to display"
        );
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: gray; font-size: 16px; padding: 40px;");
        photoGridLayout_->addWidget(emptyLabel, 0, 0, 1, 4);
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

    for (const QString& log : logs) {
        logsTextEdit_->append(log);
    }
}

void MainWindow::loadStatistics() {
    statsTable_->setRowCount(0);

    // Get actual statistics from repositories
    int totalUsers = UserRepository::getInstance().findAll().size();
    int totalPhotos = PhotoRepository::getInstance().findAll().size();

    MetricsCollector::instance().setGauge("users.total", totalUsers);
    MetricsCollector::instance().setGauge("photos.total", totalPhotos);

    int premiumUsers = 0;
    for (const auto& user : UserRepository::getInstance().findAll()) {
        if (user->getSubscriptionPackage() == SubscriptionPackage::PREMIUM ||
            user->getSubscriptionPackage() == SubscriptionPackage::GOLD) {
            premiumUsers++;
        }
    }

    qint64 totalStorage = std::accumulate(
        PhotoRepository::getInstance().findAll().cbegin(),
        PhotoRepository::getInstance().findAll().cend(),
        static_cast<qint64>(0),
        [](qint64 acc, const std::shared_ptr<Photo>& photo) { return acc + photo->getFileSize(); }
    );

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
    addRow("Total Storage Used", FileSizeFormatter::format(totalStorage));
    addRow("Logs Generated", QString::number(MetricsCollector::instance().counterValue("logs.total")));
    addRow("Uploads", QString::number(MetricsCollector::instance().counterValue("storage.uploads")));
    addRow("Photos Saved", QString::number(MetricsCollector::instance().counterValue("photos.saved")));
    addRow("Users Saved", QString::number(MetricsCollector::instance().counterValue("users.saved")));
    addRow("Search <50ms", QString::number(MetricsCollector::instance().histogramValue("searchPhotos.latency_ms.lt50ms")));
    addRow("Upload <100ms", QString::number(MetricsCollector::instance().histogramValue("uploadPhoto.latency_ms.lt100ms")));
    addRow("Avg Memory Snapshot", QString::number(MetricsCollector::instance().averageMemory("app.memory")));

    if (totalUsers > 0) {
        addRow("Average Photos per User", QString::number(totalPhotos / totalUsers));
    }
}

// ... (previous includes)

void MainWindow::handleLogout() {
    if (QMessageBox::question(this, "Logout",
        "Are you sure you want to logout?") == QMessageBox::Yes) {
        
        // Show login window again
        auto loginWindow = new LoginWindow();
        loginWindow->show();
        
        // Close current window
        close();
    }
}

// ... 

void MainWindow::addPhotoThumbnail(std::shared_ptr<Photo> photo) {
    // Create photo card widget with improved styling
    auto* photoCard = new QWidget();
    photoCard->setStyleSheet(
        "QWidget { "
        "  background: #ffffff; "
        "  border: 1px solid #e0e0e0; "
        "  border-radius: 12px; "
        "  padding: 0px; "
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

    // Edit/Details Button
    auto* detailsBtn = new QPushButton("Edit / Details");
    detailsBtn->setCursor(Qt::PointingHandCursor);
    detailsBtn->setStyleSheet(
        "QPushButton { "
        "  background-color: #0078d7; color: white; border-radius: 4px; padding: 6px; "
        "  font-weight: bold; font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #006cc1; }"
    );
    connect(detailsBtn, &QPushButton::clicked, [this, photo]() {
        PhotoDetailsDialog dlg(photo, currentUser_, &facade_, this);
        dlg.exec(); // Modal dialog
        // Refresh grid after dialog closes (in case of edits)
        loadPhotos(); 
    });
    cardLayout->addWidget(detailsBtn);

    // Add to grid layout (4 columns)
    int currentRow = photoGridLayout_->count() / 4;
    int currentCol = photoGridLayout_->count() % 4;
    photoGridLayout_->addWidget(photoCard, currentRow, currentCol);
}

//REFRESH HANDLERS

void MainWindow::refreshLogs() {
    loadSystemLogs();
}

void MainWindow::refreshStatistics() {
    MetricsCollector::instance().recordMemorySnapshot("app.memory", static_cast<qint64>(QApplication::allWidgets().size()));
    loadStatistics();
}

