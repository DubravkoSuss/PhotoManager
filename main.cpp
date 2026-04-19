#include "LoginWindow.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application metadata
    QApplication::setApplicationName("Photo Manager");
    QApplication::setApplicationVersion("1.0");
    QApplication::setOrganizationName("PhotoManager");

    // Show login window
    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}