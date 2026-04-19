#include "LoginWindow.h"
#include "MainWindow.h"
#include <QDateTime>
#include <QScreen>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
    setWindowTitle("Photo Manager - Login");
    resize(400, 300);
}

void LoginWindow::setupUI() {
    auto mainLayout = std::make_unique<QVBoxLayout>();

    // Title
    auto titleLabel = std::make_unique<QLabel>("Photo Manager");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel.release()); // Qt takes ownership

    mainLayout->addSpacing(20);

    // Form group
    auto formGroup = std::make_unique<QGroupBox>("Login");
    auto formLayout = std::make_unique<QFormLayout>();

    usernameEdit_ = new QLineEdit(this); // Parent manages lifetime
    formLayout->addRow("Username:", usernameEdit_);

    passwordEdit_ = new QLineEdit(this); // Parent manages lifetime
    passwordEdit_->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Password:", passwordEdit_);

    authProviderCombo_ = new QComboBox(this); // Parent manages lifetime
    authProviderCombo_->addItem("Local", static_cast<int>(AuthProvider::LOCAL));
    authProviderCombo_->addItem("Google", static_cast<int>(AuthProvider::GOOGLE));
    authProviderCombo_->addItem("Github", static_cast<int>(AuthProvider::GITHUB));
    formLayout->addRow("Login with:", authProviderCombo_);

    formGroup->setLayout(formLayout.release()); // Qt takes ownership
    mainLayout->addWidget(formGroup.release()); // Qt takes ownership

    // Buttons
    auto buttonLayout = std::make_unique<QHBoxLayout>();

    loginButton_ = new QPushButton("Login", this); // Parent manages lifetime
    connect(loginButton_, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    buttonLayout->addWidget(loginButton_);
    loginButton_->setDefault(true);

    registerButton_ = new QPushButton("Register", this); // Parent manages lifetime
    connect(registerButton_, &QPushButton::clicked, this, &LoginWindow::handleRegister);
    buttonLayout->addWidget(registerButton_);

    anonymousButton_ = new QPushButton("Continue as Guest", this); // Parent manages lifetime
    connect(anonymousButton_, &QPushButton::clicked, this, &LoginWindow::handleAnonymous);
    buttonLayout->addWidget(anonymousButton_);

    mainLayout->addLayout(buttonLayout.release()); // Qt takes ownership
    mainLayout->addStretch();

    setLayout(mainLayout.release()); // Qt takes ownership
}

void LoginWindow::handleLogin() {
    QString username = usernameEdit_->text();
    QString password = passwordEdit_->text();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username");
        return;
    }

    int providerIndex = authProviderCombo_->currentData().toInt();
    auto provider = static_cast<AuthProvider>(providerIndex);

    auto authService = AuthenticationFactory::getAuthService(provider);
    auto user = authService->authenticate(username, password);

    if (user) {
        Logger::getInstance().log(user->getUserId(), "User logged in");
        openMainWindow(user);
    } else {
        QMessageBox::critical(this, "Error", "Invalid credentials");
    }
}

void LoginWindow::handleRegister() {
    QMessageBox::information(this, "Register",
        "Registration dialog would open here.\nFor demo, use username 'admin' to login.");
}

void LoginWindow::handleAnonymous() {
    auto anonymous = User::Builder()
        .userId(QString("ANON_%1").arg(QDateTime::currentMSecsSinceEpoch()))
        .username("Anonymous")
        .userType(UserType::ANONYMOUS)
        .build();

    Logger::getInstance().log(anonymous->getUserId(), "Anonymous user entered");
    openMainWindow(anonymous);
}

void LoginWindow::openMainWindow(std::shared_ptr<User> user) {
    auto mainWindow = new MainWindow(user);
    mainWindow->setAttribute(Qt::WA_DeleteOnClose); // Auto-delete when closed
    mainWindow->show();
    this->close();
}