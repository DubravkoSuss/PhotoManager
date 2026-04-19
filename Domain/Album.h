#ifndef ALBUM_H
#define ALBUM_H

#include "Photo.h"
#include <QString>
#include <QList>
#include <memory>

#include "../Business Logic Layer/PhotoVisitor.h"

class AlbumComponent {
public:
    virtual ~AlbumComponent() = default;
    virtual QString name() const = 0;
    virtual void add(std::shared_ptr<AlbumComponent> child) {}
    virtual void remove(std::shared_ptr<AlbumComponent> child) {}
    virtual QList<std::shared_ptr<Photo>> collectPhotos() const = 0;
    
    // VISITOR Support
    virtual void accept(Visitor* visitor) = 0;
};

class PhotoLeaf : public AlbumComponent {
public:
    PhotoLeaf(std::shared_ptr<Photo> photo) : photo_(photo) {}
    QString name() const override { return photo_ ? photo_->getFilename() : QString(); }
    QList<std::shared_ptr<Photo>> collectPhotos() const override { return { photo_ }; }
    
    // Visitor visits this leaf
    void accept(Visitor* visitor) override {
        visitor->visitPhoto(this);
    }
    
    std::shared_ptr<Photo> getPhoto() const { return photo_; } // Expose for visitor
private:
    std::shared_ptr<Photo> photo_;
};

class AlbumComposite : public AlbumComponent {
public:
    AlbumComposite(const QString& name) : name_(name) {}
    QString name() const override { return name_; }
    void add(std::shared_ptr<AlbumComponent> child) override { children_.append(child); }
    void remove(std::shared_ptr<AlbumComponent> child) override { children_.removeOne(child); }
    QList<std::shared_ptr<Photo>> collectPhotos() const override {
        QList<std::shared_ptr<Photo>> result;
        for (auto& c : children_) {
            result.append(c->collectPhotos());
        }
        return result;
    }
    
    // Visitor visits this node, then children
    void accept(Visitor* visitor) override {
        visitor->visitAlbum(this);
        for (auto& child : children_) {
            child->accept(visitor);
        }
    }
private:
    QString name_;
    QList<std::shared_ptr<AlbumComponent>> children_;
};

#endif // ALBUM_H
