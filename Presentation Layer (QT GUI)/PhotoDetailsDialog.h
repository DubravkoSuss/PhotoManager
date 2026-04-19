#ifndef PHOTO_DETAILS_DIALOG_H
#define PHOTO_DETAILS_DIALOG_H

#include "Photo.h"
#include <QDialog>

class PhotoDetailsDialog : public QDialog {
    Q_OBJECT

public:
    PhotoDetailsDialog(std::shared_ptr<Photo> photo, QWidget* parent = nullptr)
        : QDialog(parent) {
        setWindowTitle("Photo Details");
    }
};

#endif // PHOTO_DETAILS_DIALOG_H