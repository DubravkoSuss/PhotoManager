#include "test_photo_APP.h"

void PhotoTests::serializationRoundTrip() {
    auto photo = std::make_shared<Photo>();
    photo->setPhotoId("P1");
    photo->setFilename("file.jpg");
    photo->setDescription("desc");
    photo->setHashtags({"a", "b"});
    photo->setAuthorId("U1");
    photo->setAuthorName("Tester");
    photo->setFileSize(1234);
    photo->setWidth(640);
    photo->setHeight(480);
    photo->setStoragePath("/tmp/file.jpg");

    QJsonObject json = photo->toJson();
    auto restored = Photo::fromJson(json);

    QCOMPARE(restored->getPhotoId(), QString("P1"));
    QCOMPARE(restored->getFilename(), QString("file.jpg"));
    QCOMPARE(restored->getDescription(), QString("desc"));
    QCOMPARE(restored->getAuthorId(), QString("U1"));
    QCOMPARE(restored->getHashtags().size(), 2);
}

void PhotoTests::cloneCreatesCopy() {
    auto photo = std::make_shared<Photo>();
    photo->setPhotoId("P2");
    photo->setDescription("original");
    auto clone = photo->clone();
    QVERIFY(clone->getPhotoId().contains("_copy"));
    QVERIFY(clone->getDescription().contains("Copy"));
}
