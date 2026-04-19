#ifndef TEST_THEME_H
#define TEST_THEME_H

#include <QObject>
#include <QtTest>
#include "Theme/ThemeManager.h"

class ThemeTests : public QObject {
    Q_OBJECT
private slots:
    void defaultThemeIsLight();
    void applyDoesNotCrash();
};

#endif // TEST_THEME_H
