//
//

#ifndef PHOTOMANAGER_EDITHISTORY_H
#define PHOTOMANAGER_EDITHISTORY_H

#include "PhotoMemento.h"
#include <QMap>
#include <QStack>
#include <QString>

class EditHistory {
public:
    static EditHistory& getInstance() {
        static EditHistory instance;
        return instance;
    }

    void push(const QString& photoId, const PhotoMemento& m) { history_[photoId].push(m); }
    bool canUndo(const QString& photoId) const { return history_.contains(photoId) && !history_[photoId].isEmpty(); }
    PhotoMemento pop(const QString& photoId) { return history_[photoId].pop(); }

    EditHistory(const EditHistory&) = delete;
    EditHistory& operator=(const EditHistory&) = delete;
private:
    EditHistory() = default;
    QMap<QString, QStack<PhotoMemento>> history_;
};

#endif //PHOTOMANAGER_EDITHISTORY_H
