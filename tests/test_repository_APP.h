#ifndef TEST_REPOSITORY_H
#define TEST_REPOSITORY_H

#include <QObject>
#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include "UserRepository_APP.h"
#include "PhotoRepository_APP.h"

class RepositoryTests : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void savesAndFindsUser();
    void savesAndFindsPhoto();

private:
    std::unique_ptr<QTemporaryDir> tempDir_;
};

#endif // TEST_REPOSITORY_H
