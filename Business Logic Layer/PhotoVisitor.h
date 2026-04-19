#ifndef PHOTO_VISITOR_H
#define PHOTO_VISITOR_H

#include <QString>

// Forward declarations to avoid include cycles
class PhotoLeaf;
class AlbumComposite;

/**
 * VISITOR PATTERN (Behavioral)
 * 
 * Intent: Represent an operation to be performed on the elements of an object structure.
 * Visitor lets you define a new operation without changing the classes of the elements on which it operates.
 */
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visitPhoto(PhotoLeaf* photo) = 0;
    virtual void visitAlbum(AlbumComposite* album) = 0;
};

/**
 * Concrete Visitor: StatsVisitor
 * Calculates total size and count of photos in a structure.
 */
class StatsVisitor : public Visitor {
public:
    void visitPhoto(PhotoLeaf* leaf) override;
    void visitAlbum(AlbumComposite* album) override;
    
    int getPhotoCount() const { return photoCount_; }
    int getAlbumCount() const { return albumCount_; }

private:
    int photoCount_ = 0;
    int albumCount_ = 0;
};

#endif // PHOTO_VISITOR_H
