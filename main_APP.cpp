#include "LoginWindow_APP.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // application metadata
    QApplication::setApplicationName("Photo Manager");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("PhotoManager");

    // login window
    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}