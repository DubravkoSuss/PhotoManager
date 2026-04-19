#include "AdminPanel.h"
#include "Logger_APP.h"
#include "UserRepository_APP.h"
#include "PhotoRepository_APP.h"
#include "MetricsCollector_APP.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

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
    tabWidget_->addTab(createUserManagementTab(), "User Management");
    tabWidget_->addTab(createStatsTab(), "Statistics");
    tabWidget_->addTab(createLogsTab(), "System Logs");
    
    layout->addWidget(tabWidget_);
}

QWidget* AdminPanel::createUserManagementTab() {
    auto* widget = new QWidget();
    auto* layout = new QVBoxLayout(widget);

    userTable_ = new QTableWidget();
    userTable_->setColumnCount(5);
    userTable_->setHorizontalHeaderLabels({"User ID", "Username", "Email", "Type", "Package"});
    userTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    refreshUserTable();

    layout->addWidget(userTable_);

    auto* deleteBtn = new QPushButton("Delete Selected User");
    deleteBtn->setStyleSheet("background-color: #F44336; color: white; padding: 8px; font-weight: bold; border-radius: 4px;");
    connect(deleteBtn, &QPushButton::clicked, [this]() {
        int row = userTable_->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Selection Error", "Please select a user to delete.");
            return;
        }
        
        QString userId = userTable_->item(row, 0)->text();
        QString username = userTable_->item(row, 1)->text();
        
        if (username == "admin") {
             QMessageBox::warning(this, "Action Denied", "Cannot delete the main admin account.");
             return;
        }

        auto reply = QMessageBox::question(this, "Confirm Delete", 
            QString("Are you sure you want to delete user '%1'?").arg(username),
            QMessageBox::Yes | QMessageBox::No);
            
        if (reply == QMessageBox::Yes) {
            UserRepository::getInstance().deleteUser(userId);
            refreshUserTable();
            QMessageBox::information(this, "Success", "User deleted successfully.");
        }
    });

    // EDIT USER BUTTON
    auto* editBtn = new QPushButton("Edit Selected User");
    editBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 8px; font-weight: bold; border-radius: 4px;");
    connect(editBtn, &QPushButton::clicked, [this]() {
        int row = userTable_->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Selection Error", "Please select a user to edit.");
            return;
        }

        QString userId = userTable_->item(row, 0)->text();
        auto user = UserRepository::getInstance().findById(userId);
        if(!user) return;

        // EDIT DIALOG
        QDialog dlg(this);
        dlg.setWindowTitle("Edit User");
        QFormLayout form(&dlg);

        QComboBox typeCombo;
        typeCombo.addItem("Registered", static_cast<int>(UserType::REGISTERED));
        typeCombo.addItem("Administrator", static_cast<int>(UserType::ADMINISTRATOR));
        typeCombo.setCurrentIndex(typeCombo.findData(static_cast<int>(user->getUserType())));
        form.addRow("User Type:", &typeCombo);

        QComboBox pkgCombo;
        pkgCombo.addItem("Free", static_cast<int>(SubscriptionPackage::FREE));
        pkgCombo.addItem("Pro", static_cast<int>(SubscriptionPackage::PRO));
        pkgCombo.addItem("Gold", static_cast<int>(SubscriptionPackage::GOLD));
        pkgCombo.addItem("Premium", static_cast<int>(SubscriptionPackage::PREMIUM));
        pkgCombo.setCurrentIndex(pkgCombo.findData(static_cast<int>(user->getSubscriptionPackage())));
        form.addRow("Package:", &pkgCombo);

        QDialogButtonBox bbox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&bbox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(&bbox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        form.addRow(&bbox);

        if (dlg.exec() == QDialog::Accepted) {
            user->setUserType(static_cast<UserType>(typeCombo.currentData().toInt()));
            user->setSubscriptionPackage(static_cast<SubscriptionPackage>(pkgCombo.currentData().toInt()));
            UserRepository::getInstance().save(user);
            refreshUserTable();
            QMessageBox::information(this, "Success", "User updated.");
        }
    });
    layout->addWidget(editBtn);

    layout->addWidget(deleteBtn);
    
    return widget;
}

void AdminPanel::refreshUserTable() {
    auto users = UserRepository::getInstance().findAll();
    userTable_->setRowCount(0);
    
    for (const auto& user : users) {
        int row = userTable_->rowCount();
        userTable_->insertRow(row);
        userTable_->setItem(row, 0, new QTableWidgetItem(user->getUserId()));
        userTable_->setItem(row, 1, new QTableWidgetItem(user->getUsername()));
        userTable_->setItem(row, 2, new QTableWidgetItem(user->getEmail()));
        
        QString typeStr = (user->getUserType() == UserType::ADMINISTRATOR) ? "Admin" : 
                          (user->getUserType() == UserType::ANONYMOUS) ? "Guest" : "User";
        userTable_->setItem(row, 3, new QTableWidgetItem(typeStr));
        
        QString pkgStr = (user->getSubscriptionPackage() == SubscriptionPackage::GOLD) ? "Gold" :
                         (user->getSubscriptionPackage() == SubscriptionPackage::PREMIUM) ? "Premium" : "Free";
        userTable_->setItem(row, 4, new QTableWidgetItem(pkgStr));
    }
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
    MetricsCollector::instance().setGauge("admin.users", userCount);
    MetricsCollector::instance().setGauge("admin.photos", photoCount);
    
    auto* statsGroup = new QGroupBox("System Overview");
    auto* form = new QFormLayout();
    
    auto* userLabel = new QLabel(QString::number(userCount));
    userLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2196F3;");
    form->addRow("Total Users:", userLabel);

    auto* photoLabel = new QLabel(QString::number(photoCount));
    photoLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4CAF50;");
    form->addRow("Total Photos:", photoLabel);
    
    statsGroup->setLayout(form);
    layout->addWidget(statsGroup);
    layout->addStretch();
    
    return widget;
}