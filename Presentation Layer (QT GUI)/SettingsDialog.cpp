#include "SettingsDialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include "Theme/ThemeManager.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setupUI();
    setWindowTitle("Settings");
    resize(400, 200);
}

void SettingsDialog::setupUI() {
    auto* layout = new QVBoxLayout(this);
    auto* form = new QFormLayout();

    // 1. Theme Selection
    themeCombo_ = new QComboBox(this);
    themeCombo_->addItem("Light");
    themeCombo_->addItem("Dark");
    
    // Set current selection
    AppTheme current = ThemeManager::instance().currentTheme();
    themeCombo_->setCurrentIndex(current == AppTheme::Light ? 0 : 1);
    
    connect(themeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::onThemeChanged);
    form->addRow("Theme:", themeCombo_);

    // 2. Default Export Path
    auto* pathWidget = new QWidget();
    auto* pathLayout = new QHBoxLayout(pathWidget);
    pathLayout->setContentsMargins(0,0,0,0);
    
    exportPathEdit_ = new QLineEdit("C:/Users/Dudoproton/Downloads", this); // Default or load from settings
    pathLayout->addWidget(exportPathEdit_);
    
    auto* browseBtn = new QPushButton("...", this);
    browseBtn->setFixedSize(30, 25);
    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::onBrowsePath);
    pathLayout->addWidget(browseBtn);
    
    form->addRow("Default Export Path:", pathWidget);

    layout->addLayout(form);

    auto* closeBtn = new QPushButton("Close", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeBtn);
}

void SettingsDialog::onThemeChanged(int index) {
    ThemeManager::instance().setTheme(index == 0 ? AppTheme::Light : AppTheme::Dark);
    ThemeManager::instance().apply(parentWidget()); // Apply to parent (MainWindow)
}

void SettingsDialog::onBrowsePath() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Export Directory",
                                                exportPathEdit_->text(),
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        exportPathEdit_->setText(dir);
    }
}
