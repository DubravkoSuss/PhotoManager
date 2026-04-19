#ifndef TEST_PHOTO_H
#define TEST_PHOTO_H

#include <QObject>
#include <QtTest>
#include "Photo.h"

class PhotoTests : public QObject {
    Q_OBJECT
private slots:
    void serializationRoundTrip();
    void cloneCreatesCopy();
};

#endif // TEST_PHOTO_H
