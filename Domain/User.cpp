#include "User.h"

PackageLimits User::getPackageLimits(SubscriptionPackage pkg) {
    switch (pkg) {
        case SubscriptionPackage::FREE:
            return {5 * 1024 * 1024, 10, 50};  // 5MB, 10 uploads/day, 50 photos
        case SubscriptionPackage::PRO:
            return {20 * 1024 * 1024, 50, 500}; // 20MB, 50 uploads/day, 500 photos
        case SubscriptionPackage::GOLD:
            return {100 * 1024 * 1024, -1, -1}; // 100MB, unlimited
    }
    return {0, 0, 0};
}

QString User::userTypeToString(UserType type) {
    switch (type) {
        case UserType::ANONYMOUS: return "Anonymous";
        case UserType::REGISTERED: return "Registered";
        case UserType::ADMINISTRATOR: return "Administrator";
    }
    return "Unknown";
}

QString User::authProviderToString(AuthProvider provider) {
    switch (provider) {
        case AuthProvider::LOCAL: return "Local";
        case AuthProvider::GOOGLE: return "Google";
        case AuthProvider::GITHUB: return "Github";
    }
    return "Unknown";
}

QString User::subscriptionPackageToString(SubscriptionPackage pkg) {
    switch (pkg) {
        case SubscriptionPackage::FREE: return "FREE";
        case SubscriptionPackage::PRO: return "PRO";
        case SubscriptionPackage::GOLD: return "GOLD";
    }
    return "Unknown";
}//
//