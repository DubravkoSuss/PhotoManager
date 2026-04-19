#include "test_repository_APP.h"

void RepositoryTests::initTestCase() {
    tempDir_ = std::make_unique<QTemporaryDir>();
    QVERIFY(tempDir_->isValid());
    QDir::setCurrent(tempDir_->path());
}

void RepositoryTests::savesAndFindsUser() {
    auto user = User::Builder()
        .userId("U_TEST")
        .username("tester")
        .email("tester@test.com")
        .password("Secret123")
        .userType(UserType::REGISTERED)
        .subscriptionPackage(SubscriptionPackage::FREE)
        .build();

    UserRepository::getInstance().save(user);
    auto found = UserRepository::getInstance().findById("U_TEST");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getUsername(), QString("tester"));
}

void RepositoryTests::savesAndFindsPhoto() {
    auto photo = std::make_shared<Photo>();
    photo->setPhotoId("P_TEST");
    photo->setFilename("file.jpg");
    photo->setAuthorId("U_TEST");
    photo->setAuthorName("tester");

    PhotoRepository::getInstance().save(photo);
    auto found = PhotoRepository::getInstance().findById("P_TEST");
    QVERIFY(found != nullptr);
    QCOMPARE(found->getFilename(), QString("file.jpg"));
}
