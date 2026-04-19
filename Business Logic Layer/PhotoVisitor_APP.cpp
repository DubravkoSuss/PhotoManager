#include "PhotoVisitor.h"
#include "../Domain/Album.h"
#include "../Infrastructure layer/Logger_APP.h"

void StatsVisitor::visitPhoto(PhotoLeaf* leaf) {
    // We access the photo data from the leaf
    photoCount_++;
}

void StatsVisitor::visitAlbum(AlbumComposite* album) {
    // Just visiting the album node itself
    albumCount_++;
    Logger::getInstance().log("StatsVisitor", "Visiting Album: " + album->name());
}
