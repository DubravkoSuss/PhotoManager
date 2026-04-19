#include <QApplication>
#include <QtTest>

#include "test_user_APP.h"
#include "test_photo_APP.h"
#include "test_search_APP.h"
#include "test_repository_APP.h"
#include "test_facade_APP.h"
#include "test_aspect_metrics_APP.h"
#include "test_ui_APP.h"
#include "test_theme_APP.h"
#include "test_search_dialog_APP.h"
#include "test_upload_dialog_APP.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    int status = 0;
    status |= QTest::qExec(new UserTests, argc, argv);
    status |= QTest::qExec(new PhotoTests, argc, argv);
    status |= QTest::qExec(new SearchTests, argc, argv);
    status |= QTest::qExec(new RepositoryTests, argc, argv);
    status |= QTest::qExec(new FacadeTests, argc, argv);
    status |= QTest::qExec(new AspectMetricsTests, argc, argv);
    status |= QTest::qExec(new UiTests, argc, argv);
    status |= QTest::qExec(new ThemeTests, argc, argv);
    status |= QTest::qExec(new SearchDialogTests, argc, argv);
    status |= QTest::qExec(new UploadDialogTests, argc, argv);
    return status;
}
