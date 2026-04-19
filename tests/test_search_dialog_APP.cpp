#include "test_search_dialog_APP.h"

void SearchDialogTests::dialogCreatesSearchButton() {
    PhotoManagementFacade facade;
    SearchDialog dialog(&facade);
    dialog.show();

    auto buttons = dialog.findChildren<QPushButton*>();
    bool found = false;
    for (auto* btn : buttons) {
        if (btn->text().contains("Search")) found = true;
    }
    QVERIFY(found);
}
