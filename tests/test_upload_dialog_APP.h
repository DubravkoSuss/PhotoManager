#ifndef TEST_UPLOAD_DIALOG_H
#define TEST_UPLOAD_DIALOG_H

#include <QObject>
#include <QtTest>
#include "UploadDialog_APP.h"
#include "Command_APP.h"

class UploadDialogTests : public QObject {
    Q_OBJECT
private slots:
    void dialogHasRequiredControls();
};

#endif // TEST_UPLOAD_DIALOG_H
