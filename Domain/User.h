#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <memory>
#include <QJsonObject>
#include <QJsonArray>

enum class UserType {
    ANONYMOUS,
    REGISTERED,
    ADMINISTRATOR
};

enum class AuthProvider {
    LOCAL,
    GOOGLE,
    GITHUB
};

enum class SubscriptionPackage {
    FREE,
    PRO,
    GOLD,
    PREMIUM
};

struct PackageLimits {
    qint64 maxUploadSize;
    int dailyUploadLimit;
    int maxTotalPhotos;
};

class User {
public:
    class Builder;

    QString getUserId() const { return userId_; }
    QString getUsername() const { return username_; }
    QString getEmail() const { return email_; }
    QString getPasswordHash() const { return passwordHash_; } // New
    UserType getUserType() const { return userType_; }
    SubscriptionPackage getSubscriptionPackage() const { return subscriptionPackage_; }
    QDateTime getRegistrationDate() const { return registrationDate_; }
    AuthProvider getAuthProvider() const { return authProvider_; }

    void setSubscriptionPackage(SubscriptionPackage pkg) { subscriptionPackage_ = pkg; }
    void setUserId(const QString& id) { userId_ = id; }
    void setUsername(const QString& name) { username_ = name; }
    void setPasswordHash(const QString& hash) { passwordHash_ = hash; }
    void setEmail(const QString& email) { email_ = email; }
    void setRegistrationDate(const QDateTime& dt) { registrationDate_ = dt; }
    void setUserType(UserType type) { userType_ = type; }
    void setAuthProvider(AuthProvider prof) { authProvider_ = prof; }

    static PackageLimits getPackageLimits(SubscriptionPackage pkg);
    static QString userTypeToString(UserType type);
    static QString authProviderToString(AuthProvider provider);
    static QString subscriptionPackageToString(SubscriptionPackage pkg);

    // Serialization
    QJsonObject toJson() const {
        QJsonObject json;
        json["id"] = userId_;
        json["username"] = username_;
        json["email"] = email_;
        json["password"] = passwordHash_;
        json["type"] = static_cast<int>(userType_);
        json["pkg"] = static_cast<int>(subscriptionPackage_);
        json["auth"] = static_cast<int>(authProvider_);
        json["regDate"] = registrationDate_.toString(Qt::ISODate);
        return json;
    }

    static std::shared_ptr<User> fromJson(const QJsonObject& json) {
        auto user = std::shared_ptr<User>(new User(
            json["id"].toString(),
            json["username"].toString(),
            json["email"].toString(),
            static_cast<UserType>(json["type"].toInt()),
            static_cast<SubscriptionPackage>(json["pkg"].toInt()),
            static_cast<AuthProvider>(json["auth"].toInt())
        ));
        user->setPasswordHash(json["password"].toString());
        user->setRegistrationDate(QDateTime::fromString(json["regDate"].toString(), Qt::ISODate));
        return user;
    }

    int getUploadCount() const { return uploadCount_; }
    void incrementUploadCount() { uploadCount_++; }
    void setUploadCount(int count) { uploadCount_ = count; }

private:
    QString userId_;
    QString username_;
    QString email_;
    QString passwordHash_;
    UserType userType_;
    SubscriptionPackage subscriptionPackage_;
    QDateTime registrationDate_;
    AuthProvider authProvider_;
    int uploadCount_ = 0; // Usage tracker

    User(const QString& userId, const QString& username, const QString& email,
         UserType userType, SubscriptionPackage pkg, AuthProvider provider)
        : userId_(userId), username_(username), email_(email),
          userType_(userType), subscriptionPackage_(pkg),
          registrationDate_(QDateTime::currentDateTime()), authProvider_(provider),
          uploadCount_(0) {}

    friend class Builder;
};

// Builder Pattern
class User::Builder {
public:
    Builder() : userType_(UserType::REGISTERED),
                subscriptionPackage_(SubscriptionPackage::FREE),
                authProvider_(AuthProvider::LOCAL) {}

    Builder& userId(const QString& id) { userId_ = id; return *this; }
    Builder& username(const QString& name) { username_ = name; return *this; }
    Builder& email(const QString& email) { email_ = email; return *this; }
    Builder& password(const QString& pass) { password_ = pass; return *this; } // New
    Builder& userType(UserType type) { userType_ = type; return *this; }
    Builder& subscriptionPackage(SubscriptionPackage pkg) { subscriptionPackage_ = pkg; return *this; }
    Builder& authProvider(AuthProvider provider) { authProvider_ = provider; return *this; }

    std::shared_ptr<User> build() {
        auto user = std::shared_ptr<User>(new User(userId_, username_, email_,
                                              userType_, subscriptionPackage_, authProvider_));
        user->setPasswordHash(password_);
        return user;
    }

private:
    QString userId_;
    QString username_;
    QString email_;
    QString password_;
    UserType userType_;
    SubscriptionPackage subscriptionPackage_;
    AuthProvider authProvider_;
};

#endif // USER_H