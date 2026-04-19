#ifndef ADMIN_PANEL_H
#define ADMIN_PANEL_H

#include "User.h"
#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QFormLayout>

class AdminPanel : public QWidget {
    Q_OBJECT

public:
    AdminPanel(std::shared_ptr<User> adminUser, QWidget* parent = nullptr);

private:
    void setupUI();
    QWidget* createLogsTab();
    QWidget* createStatsTab();
    QWidget* createUserManagementTab();
    void refreshUserTable();

    std::shared_ptr<User> adminUser_;
    QTabWidget* tabWidget_;
    QTableWidget* userTable_;
};

#endif // ADMIN_PANEL_H