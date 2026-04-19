#include "LoginWindow_APP.h"
#include "MainWindow_APP.h"
#include <QDateTime>
#include <QScreen>
#include <QPainter>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setupUI();
    setWindowTitle("Photo Manager - Login");
    resize(400, 300);
}

#include <QRegularExpression>
#include "UserRepository_APP.h"

// ... 

// Helper to create simple icons
QIcon createColorIcon(const QColor& color, const QString& text) {
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(2, 2, 28, 28);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    return QIcon(pixmap);
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
    mainLayout->addWidget(titleLabel.release()); 

    mainLayout->addSpacing(30);

    // Form group
    auto formGroup = std::make_unique<QGroupBox>("Login");
    auto formLayout = std::make_unique<QFormLayout>();
    formLayout->setContentsMargins(20, 30, 20, 20); // Add top margin to avoid title overlap
    formLayout->setVerticalSpacing(15);

    usernameEdit_ = new QLineEdit(this);
    usernameEdit_->setPlaceholderText("Username (3+ chars)");
    formLayout->addRow("Username:", usernameEdit_);

    // Register-only fields
    emailEdit_ = new QLineEdit(this);
    emailEdit_->setPlaceholderText("example@email.com");
    emailEdit_->setVisible(false); // Hidden by default
    formLayout->addRow("Email:", emailEdit_);

    packageCombo_ = new QComboBox(this);
    packageCombo_->addItem("Free", static_cast<int>(SubscriptionPackage::FREE));
    packageCombo_->addItem("Pro", static_cast<int>(SubscriptionPackage::PRO));
    packageCombo_->addItem("Gold", static_cast<int>(SubscriptionPackage::GOLD));
    packageCombo_->addItem("Premium", static_cast<int>(SubscriptionPackage::PREMIUM));
    packageCombo_->setVisible(false);
    formLayout->addRow("Package:", packageCombo_);

    passwordEdit_ = new QLineEdit(this);
    passwordEdit_->setEchoMode(QLineEdit::Password);
    passwordEdit_->setPlaceholderText("Password");
    formLayout->addRow("Password:", passwordEdit_);

    // PROVIDER SELECTION (Icons instead of ComboBox)
    providerContainer_ = new QWidget(this);
    auto providerLayout = new QHBoxLayout(providerContainer_);
    providerLayout->setContentsMargins(0, 0, 0, 0); // Tight fit
    
    providerGroup_ = new QButtonGroup(this);
    providerGroup_->setExclusive(true);

    auto createProviderBtn = [&](AuthProvider type, const QString& name, const QColor& color, const QString& symbol) {
        auto btn = new QPushButton(name, this);
        btn->setCheckable(true);
        btn->setIcon(createColorIcon(color, symbol));
        btn->setIconSize(QSize(24, 24));
        btn->setCursor(Qt::PointingHandCursor);
        if (type == AuthProvider::LOCAL) btn->setChecked(true); // Default
        
        providerGroup_->addButton(btn, static_cast<int>(type));
        providerLayout->addWidget(btn);
        
        connect(btn, &QPushButton::clicked, [this, type]() {
            selectedProvider_ = type;
        });
        return btn;
    };

    createProviderBtn(AuthProvider::LOCAL, "Local", QColor("#2196F3"), "L");
    createProviderBtn(AuthProvider::GOOGLE, "Google", QColor("#DB4437"), "G");
    createProviderBtn(AuthProvider::GITHUB, "Github", QColor("#333333"), "gh");

    formLayout->addRow("Login with:", providerContainer_);
    
    // Initial State: Hide Email/Package Rows
    formLayout->setRowVisible(emailEdit_, false);
    formLayout->setRowVisible(packageCombo_, false);

    formGroup->setLayout(formLayout.release());
    mainLayout->addWidget(formGroup.release());

    // Buttons
    auto buttonLayout = std::make_unique<QHBoxLayout>();

    loginButton_ = new QPushButton("Login", this);
    connect(loginButton_, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    buttonLayout->addWidget(loginButton_);
    loginButton_->setDefault(true);

    // Register toggle button
    registerButton_ = new QPushButton("Switch to Register", this);
    connect(registerButton_, &QPushButton::clicked, this, &LoginWindow::toggleRegisterMode);
    buttonLayout->addWidget(registerButton_);

    // Actual Register Submit Button (Hidden by default)
    submitRegisterButton_ = new QPushButton("Create Account", this);
    submitRegisterButton_->setVisible(false);
    connect(submitRegisterButton_, &QPushButton::clicked, this, &LoginWindow::performRegister);
    buttonLayout->addWidget(submitRegisterButton_);

    anonymousButton_ = new QPushButton("Continue as Guest", this);
    connect(anonymousButton_, &QPushButton::clicked, this, &LoginWindow::handleAnonymous);
    buttonLayout->addWidget(anonymousButton_);

    mainLayout->addLayout(buttonLayout.release()); 
    mainLayout->addStretch();

    setLayout(mainLayout.release()); 
}

void LoginWindow::handleLogin() {
    if (isRegisterMode_) return;

    QString username = usernameEdit_->text().trimmed();
    QString password = passwordEdit_->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter username and password");
        return;
    }

    // Check Local Auth via Repository
    if (selectedProvider_ == AuthProvider::LOCAL) {
        auto user = UserRepository::getInstance().findByUsernameAndPassword(username, password);
        if (user) {
            Logger::getInstance().log(user->getUserId(), "User logged in");
            openMainWindow(user);
        } else {
            QMessageBox::critical(this, "Error", "Invalid username or password");
        }
    } else {
        // External providers (Mock)
        auto user = AuthenticationFactory::getAuthService(selectedProvider_)
                        ->authenticate(username, password);
        if(user) openMainWindow(user);
    }
}

void LoginWindow::toggleRegisterMode() {
    isRegisterMode_ = !isRegisterMode_;
    
    // Toggle UI state
    auto form = static_cast<QFormLayout*>(this->findChild<QGroupBox*>()->layout());
    
    form->setRowVisible(emailEdit_, isRegisterMode_); // Show/Hide Email Row
    form->setRowVisible(packageCombo_, isRegisterMode_); // Show/Hide Package Row
    form->setRowVisible(providerContainer_, !isRegisterMode_); // Show/Hide Provider Row

    loginButton_->setVisible(!isRegisterMode_);
    submitRegisterButton_->setVisible(isRegisterMode_);
    
    if (isRegisterMode_) {
        registerButton_->setText("Back to Login");
        usernameEdit_->setPlaceholderText("Choose Username (3+ chars)");
        passwordEdit_->setPlaceholderText("Min 8 chars, 1 uppercase, 1 digit");
        static_cast<QGroupBox*>(this->layout()->itemAt(2)->widget())->setTitle("Register");
    } else {
        registerButton_->setText("Switch to Register");
        usernameEdit_->setPlaceholderText("Username");
        passwordEdit_->setPlaceholderText("Password");
        static_cast<QGroupBox*>(this->layout()->itemAt(2)->widget())->setTitle("Login");
    }
}

void LoginWindow::handleRegister() {
    toggleRegisterMode(); // redirect to toggle
}

void LoginWindow::performRegister() {
    QString username = usernameEdit_->text().trimmed();
    QString password = passwordEdit_->text();
    QString email = emailEdit_->text().trimmed();

    
    static const QRegularExpression userRgx(R"(^[a-zA-Z0-9_]{3,20}$)");
    if (!userRgx.match(username).hasMatch()) {
        QMessageBox::warning(this, "Invalid Username", 
            "Username must be 3-20 characters, alphanumeric only.");
        return;
    }
    
    // Email: Standard regex
    static const QRegularExpression emailRgx(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    if (!emailRgx.match(email).hasMatch()) {
        QMessageBox::warning(this, "Invalid Email", "Please enter a valid email address.");
        return;
    }
    
    // Password: 8+ chars, 1 upper, 1 digit.
    static const QRegularExpression passRgx(R"(^(?=.*[A-Z])(?=.*\d).{8,}$)");
    if (!passRgx.match(password).hasMatch()) {
        QMessageBox::warning(this, "Weak Password", 
            "Password must have at least 8 characters, include 1 uppercase letter and 1 number.");
        return;
    }
    
    // Check if user exists
    if (UserRepository::getInstance().findByUsername(username)) {
         QMessageBox::warning(this, "Error", "Username already taken.");
         return;
    }
    
    // Get Package
    auto pkg = static_cast<SubscriptionPackage>(packageCombo_->currentData().toInt());

    // Create User
    auto newUser = User::Builder()
        .userId(QString("USER_%1").arg(QDateTime::currentMSecsSinceEpoch()))
        .username(username)
        .password(password)
        .email(email)
        .userType(UserType::REGISTERED)
        .subscriptionPackage(pkg)
        .build();
        
    // Save to Persistent Repo
    UserRepository::getInstance().save(newUser);
    
    QMessageBox::information(this, "Success", "Account created successfully!");
    
    // Determine Login
    Logger::getInstance().log(newUser->getUserId(), "New user registered");
    openMainWindow(newUser);
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