#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QString>
#include <QSettings>
#include <QApplication>
#include <QWidget>

enum class AppTheme { Light, Dark };

class ThemeManager {
public:
    static ThemeManager& instance();
    void apply(QWidget* root);
    void setTheme(AppTheme theme);
    AppTheme currentTheme() const;
private:
    ThemeManager();
    QString stylesheetFor(AppTheme theme) const;
    AppTheme theme_;
};

#endif // THEME_MANAGER_H
