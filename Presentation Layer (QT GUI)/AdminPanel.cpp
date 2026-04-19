#include "AdminPanel.h"
#include "Logger.h"
#include "UserRepository.h"
#include "PhotoRepository.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>

AdminPanel::AdminPanel(std::shared_ptr<User> adminUser, QWidget* parent)
    : QWidget(parent), adminUser_(adminUser) {
    setupUI();
    setWindowTitle("Admin Panel");
    resize(700, 400);
    QPoint currentPos = pos();
    move(0,100);
}

void AdminPanel::setupUI() {
    auto* layout = new QVBoxLayout(this);
    
    tabWidget_ = new QTabWidget(this);
    tabWidget_->addTab(createLogsTab(), "System Logs");
    tabWidget_->addTab(createStatsTab(), "Statistics");
    
    layout->addWidget(tabWidget_);
}

QWidget* AdminPanel::createLogsTab() {
    auto* widget = new QWidget();
    auto* layout = new QVBoxLayout(widget);
    
    auto* logsText = new QTextEdit(widget);
    logsText->setReadOnly(true);
    
    auto logs = Logger::getInstance().getLogs();
    logsText->setPlainText(logs.join("\n"));
    
    layout->addWidget(logsText);
    return widget;
}

QWidget* AdminPanel::createStatsTab() {
    auto* widget = new QWidget();
    auto* layout = new QVBoxLayout(widget);
    
    int userCount = UserRepository::getInstance().findAll().size();
    int photoCount = PhotoRepository::getInstance().findAll().size();
    
    layout->addWidget(new QLabel(QString("Total Users: %1").arg(userCount)));
    layout->addWidget(new QLabel(QString("Total Photos: %1").arg(photoCount)));
    layout->addStretch();
    
    return widget;
}