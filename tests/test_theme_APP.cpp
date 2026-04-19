#include "test_theme_APP.h"

void ThemeTests::defaultThemeIsLight() {
    QCOMPARE(ThemeManager::instance().currentTheme(), AppTheme::Light);
}

void ThemeTests::applyDoesNotCrash() {
    QWidget widget;
    ThemeManager::instance().apply(&widget);
    QVERIFY(true);
}
