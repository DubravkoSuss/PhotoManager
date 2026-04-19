//
//

#ifndef PHOTOMANAGER_PHOTOMEMENTO_H
#define PHOTOMANAGER_PHOTOMEMENTO_H

#include "Photo.h"
#include <QString>
#include <QStringList>

class PhotoMemento {
public:
    PhotoMemento(const QString& description,
                 const QStringList& hashtags)
        : description_(description), hashtags_(hashtags) {}
    const QString& description() const { return description_; }
    const QStringList& hashtags() const { return hashtags_; }
private:
    QString description_;
    QStringList hashtags_;
};

#endif //PHOTOMANAGER_PHOTOMEMENTO_H
