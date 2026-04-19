#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include "User.h"
#include "AuthenticationFactory_APP.h"
#include "Logger_APP.h"
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QButtonGroup>
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
    QPushButton* loginButton_;
    QPushButton* registerButton_; // Used for toggle
    QPushButton* submitRegisterButton_; // Used to submit registration
    QPushButton* anonymousButton_;
    QButtonGroup* providerGroup_;
    QWidget* providerContainer_;
    AuthProvider selectedProvider_ = AuthProvider::LOCAL;
    
    // Registration fields
    QLineEdit* emailEdit_;
    QComboBox* packageCombo_;
    
    bool isRegisterMode_ = false;
    void toggleRegisterMode();
    void performRegister();
};

#endif // LOGIN_WINDOW_H