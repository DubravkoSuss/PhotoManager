#include "test_upload_dialog_APP.h"

void UploadDialogTests::dialogHasRequiredControls() {
    auto user = User::Builder()
        .userId("U_DIALOG")
        .username("dialog")
        .email("dialog@test.com")
        .password("Secret123")
        .userType(UserType::REGISTERED)
        .subscriptionPackage(SubscriptionPackage::FREE)
        .build();

    PhotoManagementFacade facade;
    CommandInvoker invoker;
    UploadDialog dialog(user, &facade, &invoker);
    dialog.show();

    auto lineEdits = dialog.findChildren<QLineEdit*>();
    auto textEdits = dialog.findChildren<QTextEdit*>();
    auto checkboxes = dialog.findChildren<QCheckBox*>();

    QVERIFY(lineEdits.size() >= 2);
    QVERIFY(textEdits.size() >= 1);
    QVERIFY(checkboxes.size() >= 3);
}
