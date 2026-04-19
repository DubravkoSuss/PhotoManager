#include "test_ui_APP.h"

void UiTests::loginWindowToggleRegister() {
    LoginWindow window;
    window.show();

    auto buttons = window.findChildren<QPushButton*>();
    QPushButton* toggle = nullptr;
    QPushButton* createAccount = nullptr;
    for (auto* btn : buttons) {
        if (btn->text().contains("Register")) toggle = btn;
        if (btn->text().contains("Create Account")) createAccount = btn;
    }

    QVERIFY(toggle != nullptr);
    QVERIFY(createAccount != nullptr);
    QVERIFY(!createAccount->isVisible());

    QTest::mouseClick(toggle, Qt::LeftButton);
    QVERIFY(createAccount->isVisible());
}
