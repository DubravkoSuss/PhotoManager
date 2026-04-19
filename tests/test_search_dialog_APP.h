#ifndef TEST_SEARCH_DIALOG_H
#define TEST_SEARCH_DIALOG_H

#include <QObject>
#include <QtTest>
#include "SearchDialog_APP.h"
#include "PhotoManagementFacade_APP.h"

class SearchDialogTests : public QObject {
    Q_OBJECT
private slots:
    void dialogCreatesSearchButton();
};

#endif // TEST_SEARCH_DIALOG_H
