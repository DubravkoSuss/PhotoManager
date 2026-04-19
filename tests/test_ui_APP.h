#ifndef TEST_UI_H
#define TEST_UI_H

#include <QObject>
#include <QtTest>
#include "LoginWindow_APP.h"

class UiTests : public QObject {
    Q_OBJECT
private slots:
    void loginWindowToggleRegister();
};

#endif // TEST_UI_H
