#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "User.h"
#include "PhotoManagementFacade.h"
#include "Command.h"
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QGridLayout>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <memory>

/**
 * Main Application Window - MVC Pattern
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(std::shared_ptr<User> user, QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void handleUpload();
    void handleSearch();
    void handleAdmin();
    void handleLogout();
    void returnToGallery();

private:
    // Setup methods
    void setupUI();
    void setupGalleryView();    // NEW: Setup main gallery page
    void setupAdminView();      // NEW: Setup admin panel page

    // Gallery methods
    void loadPhotos();
    void addPhotoThumbnail(std::shared_ptr<Photo> photo);

    // Admin methods
    void loadSystemLogs();
    void loadStatistics();
    void refreshLogs();
    void refreshStatistics();
    QString formatFileSize(qint64 size);

    std::shared_ptr<User> currentUser_;
    PhotoManagementFacade facade_;
    CommandInvoker invoker_;

    QLabel* userInfoLabel_;
    QPushButton* uploadButton_;
    QPushButton* searchButton_;
    QPushButton* adminButton_;
    QPushButton* logoutButton_;
    QPushButton* backButton_;

    // Main navigation
    QStackedWidget* stackedWidget_;  // NEW: Manages different views

    // Gallery view widgets (Page 0)
    QWidget* galleryPage_;
    QWidget* photoGridWidget_;
    QGridLayout* photoGridLayout_;

    // Admin view widgets (Page 1)
    QWidget* adminPage_;
    QTabWidget* adminTabWidget_;
    QTextEdit* logsTextEdit_;
    QTableWidget* statsTable_;


};

#endif // MAIN_WINDOW_H