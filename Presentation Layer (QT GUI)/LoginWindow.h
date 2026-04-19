#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include "User.h"
#include "AuthenticationFactory.h"
#include "Logger.h"
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFormLayout>
#include <QGroupBox>
#include <memory>

class MainWindow;

/**
 * Login Window - MVC Pattern (View)
 */
class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override = default;

private slots:
    void handleLogin();
    void handleRegister();
    void handleAnonymous();

private:
    void setupUI();
    void openMainWindow(std::shared_ptr<User> user);

    QLineEdit* usernameEdit_;
    QLineEdit* passwordEdit_;
    QComboBox* authProviderCombo_;
    QPushButton* loginButton_;
    QPushButton* registerButton_;
    QPushButton* anonymousButton_;
};

#endif // LOGIN_WINDOW_H