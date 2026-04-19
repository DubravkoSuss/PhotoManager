#include "test_facade_APP.h"

void FacadeTests::initTestCase() {
    tempDir_ = std::make_unique<QTemporaryDir>();
    QVERIFY(tempDir_->isValid());
    QDir::setCurrent(tempDir_->path());
}

void FacadeTests::uploadFailsForMissingFile() {
    PhotoManagementFacade facade;
    auto user = User::Builder()
        .userId("U1")
        .username("tester")
        .email("tester@test.com")
        .password("Secret123")
        .userType(UserType::REGISTERED)
        .subscriptionPackage(SubscriptionPackage::FREE)
        .build();

    auto processor = std::make_shared<BaseImageProcessor>();
    auto result = facade.uploadPhoto(user, "missing.jpg", "desc", {"tag"}, processor);
    QVERIFY(result == nullptr);
}

void FacadeTests::updatePhotoStoresHistory() {
    PhotoManagementFacade facade;
    auto user = User::Builder()
        .userId("U2")
        .username("tester2")
        .email("tester2@test.com")
        .password("Secret123")
        .userType(UserType::REGISTERED)
        .subscriptionPackage(SubscriptionPackage::FREE)
        .build();

    auto photo = std::make_shared<Photo>();
    photo->setPhotoId("PHOTO_1");
    photo->setDescription("old");
    photo->setHashtags({"old"});
    photo->setAuthorId(user->getUserId());
    PhotoRepository::getInstance().save(photo);

    facade.updatePhoto(user, "PHOTO_1", "new", {"new"});
    facade.undoLastEdit(user, "PHOTO_1");

    auto updated = PhotoRepository::getInstance().findById("PHOTO_1");
    QCOMPARE(updated->getDescription(), QString("old"));
}
