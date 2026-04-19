#include "ThemeManager.h"

ThemeManager& ThemeManager::instance() {
    static ThemeManager inst;
    return inst;
}

ThemeManager::ThemeManager() {
    // Always default to Light
    theme_ = AppTheme::Light;
}

void ThemeManager::apply(QWidget* root) {
    qApp->setStyleSheet(stylesheetFor(theme_));
    if (root) root->update();
}

void ThemeManager::setTheme(AppTheme theme) {
    // Force Light theme
    theme_ = AppTheme::Light;
    QSettings s("PhotoManager", "App");
    s.setValue("theme", "light");
}

AppTheme ThemeManager::currentTheme() const {
    return AppTheme::Light;
}

QString ThemeManager::stylesheetFor(AppTheme theme) const {
    return
        "QWidget { background-color: #f7f7f7; color: #222; }"
        "QGroupBox, QFrame { border: 1px solid #ddd; border-radius: 8px; }"
        "QLabel { color: #222; }"
        "QPushButton { "
        "  background-color: #ffffff; "
        "  color: #222; "
        "  border: 1px solid #ccc; "
        "  padding: 8px 16px; "
        "  border-radius: 6px; "
        "  font-weight: bold; "
        "}"
        "QPushButton:hover { background-color: #f0f0f0; border: 1px solid #bbb; }"
        "QPushButton:pressed { background-color: #e0e0e0; }"
        "QLineEdit, QComboBox, QTextEdit { background-color: #ffffff; color: #222; border: 1px solid #ccc; border-radius: 6px; }"
        "QCheckBox { spacing: 5px; }"
        "QCheckBox::indicator { width: 18px; height: 18px; border: 1px solid #ccc; border-radius: 3px; background-color: white; }"
        "QCheckBox::indicator:checked { background-color: #2196F3; border: 1px solid #2196F3; image: url(:/icons/check.png); }" // Basic style, if no image, color change is enough.
        // Actually, without an image resource, let's just use a color or standard look.
        // A simple border usually fixes "invisible" boxes against gray backgrounds.
        "QCheckBox::indicator:checked { background-color: #2196F3; border: 1px solid #2196F3; }"
        "QTabWidget::pane { border: 1px solid #ccc; }"
        "QHeaderView::section { background-color: #eaeaea; color: #222; padding: 6px; border: none; }";
}
