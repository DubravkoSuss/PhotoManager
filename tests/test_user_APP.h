#ifndef TEST_USER_H
#define TEST_USER_H

#include <QObject>
#include <QtTest>
#include "User.h"

class UserTests : public QObject {
    Q_OBJECT
private slots:
    void builderCreatesUser();
    void packageLimitsForFree();
};

#endif // TEST_USER_H
