#ifndef TEST_SEARCH_H
#define TEST_SEARCH_H

#include <QObject>
#include <QtTest>
#include "PhotoSearchCriteria_APP.h"

class SearchTests : public QObject {
    Q_OBJECT
private slots:
    void matchesByHashtag();
    void rejectsBySize();
};

#endif // TEST_SEARCH_H
