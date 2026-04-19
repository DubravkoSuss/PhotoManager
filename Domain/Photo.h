#ifndef PHOTO_H
#define PHOTO_H

#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

class Photo {
public:
    Photo() : uploadDateTime_(QDateTime::currentDateTime()), fileSize_(0), width_(0), height_(0) {}

    // Getters
    // PROTOTYPE PATTERN (Creational)
    virtual std::shared_ptr<Photo> clone() const {
        // Deep copy of the object
        auto newPhoto = std::make_shared<Photo>(*this);
        newPhoto->setPhotoId(photoId_ + "_copy"); // Ensure unique ID for the copy
        newPhoto->setDescription(description_ + " (Copy)");
        return newPhoto;
    }

    QString getPhotoId() const { return photoId_; }
    QString getFilename() const { return filename_; }
    QString getDescription() const { return description_; }
    QStringList getHashtags() const { return hashtags_; }
    QString getAuthorId() const { return authorId_; }
    QString getAuthorName() const { return authorName_; }
    QDateTime getUploadDateTime() const { return uploadDateTime_; }
    qint64 getFileSize() const { return fileSize_; }
    QString getFormat() const { return format_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    QString getStoragePath() const { return storagePath_; }
    QString getThumbnailPath() const { return thumbnailPath_; }
    enum class State { NEW, PROCESSING, PUBLISHED, ARCHIVED };
    State getState() const { return state_; }

    // Setters
    void setPhotoId(const QString& id) { photoId_ = id; }
    void setFilename(const QString& name) { filename_ = name; }
    void setDescription(const QString& desc) { description_ = desc; }
    void setHashtags(const QStringList& tags) { hashtags_ = tags; }
    void setAuthorId(const QString& id) { authorId_ = id; }
    void setAuthorName(const QString& name) { authorName_ = name; }
    void setUploadDateTime(const QDateTime& dt) { uploadDateTime_ = dt; }
    void setFileSize(qint64 size) { fileSize_ = size; }
    void setFormat(const QString& fmt) { format_ = fmt; }


    void setWidth(int w) { width_ = w; }
    void setHeight(int h) { height_ = h; }
    void setStoragePath(const QString& path) { storagePath_ = path; }
    void setThumbnailPath(const QString& path) { thumbnailPath_ = path; }
    void setState(State s) { state_ = s; }

    // Serialization
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = photoId_;
        json["filename"] = filename_;
        json["description"] = description_;
        json["hashtags"] = QJsonArray::fromStringList(hashtags_);
        json["authorId"] = authorId_;
        json["authorName"] = authorName_;
        json["uploadDate"] = uploadDateTime_.toString(Qt::ISODate);
        json["size"] = fileSize_;
        json["width"] = width_;
        json["height"] = height_;
        json["path"] = storagePath_;
        return json;
    }

    static std::shared_ptr<Photo> fromJson(const QJsonObject& json) {
        auto photo = std::make_shared<Photo>();
        photo->setPhotoId(json["id"].toString());
        photo->setFilename(json["filename"].toString());
        photo->setDescription(json["description"].toString());
        
        QStringList tags;
        for (const auto& val : json["hashtags"].toArray()) {
            tags.append(val.toString());
        }
        photo->setHashtags(tags);
        
        photo->setAuthorId(json["authorId"].toString());
        photo->setAuthorName(json["authorName"].toString());
        photo->setUploadDateTime(QDateTime::fromString(json["uploadDate"].toString(), Qt::ISODate));
        photo->setFileSize(static_cast<qint64>(json["size"].toDouble())); // Cast to qint64
        photo->setWidth(json["width"].toInt());
        photo->setHeight(json["height"].toInt());
        photo->setStoragePath(json["path"].toString());
        photo->setState(State::PUBLISHED); // Default state when deserializing
        return photo;
    }

private:
    QString photoId_;
    QString filename_;
    QString description_;
    QStringList hashtags_;
    QString authorId_;
    QString authorName_;
    QDateTime uploadDateTime_;
    qint64 fileSize_;
    QString format_;
    int width_;
    int height_;
    QString storagePath_;
    QString thumbnailPath_;
    State state_ = State::NEW;
};

#endif // PHOTO_H
