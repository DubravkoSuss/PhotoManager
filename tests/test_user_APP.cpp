#include "test_user_APP.h"

void UserTests::builderCreatesUser() {
    auto user = User::Builder()
        .userId("U1")
        .username("tester")
        .email("test@example.com")
        .password("Secret123")
        .userType(UserType::REGISTERED)
        .subscriptionPackage(SubscriptionPackage::PRO)
        .build();

    QCOMPARE(user->getUserId(), QString("U1"));
    QCOMPARE(user->getUsername(), QString("tester"));
    QCOMPARE(user->getEmail(), QString("test@example.com"));
    QCOMPARE(user->getPasswordHash(), QString("Secret123"));
    QCOMPARE(user->getSubscriptionPackage(), SubscriptionPackage::PRO);
}

void UserTests::packageLimitsForFree() {
    PackageLimits limits = User::getPackageLimits(SubscriptionPackage::FREE);
    QVERIFY(limits.maxUploadSize > 0);
    QVERIFY(limits.dailyUploadLimit > 0);
    QVERIFY(limits.maxTotalPhotos > 0);
}
