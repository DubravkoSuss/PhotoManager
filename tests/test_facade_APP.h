#ifndef TEST_FACADE_H
#define TEST_FACADE_H

#include <QObject>
#include <QtTest>
#include <QTemporaryDir>
#include <QDir>
#include "PhotoManagementFacade_APP.h"
#include "ImageProcessor_APP.h"

class FacadeTests : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void uploadFailsForMissingFile();
    void updatePhotoStoresHistory();

private:
    std::unique_ptr<QTemporaryDir> tempDir_;
};

#endif // TEST_FACADE_H
