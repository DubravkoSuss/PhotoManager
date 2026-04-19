#ifndef AUTHENTICATION_SERVICE_H
#define AUTHENTICATION_SERVICE_H

#include "User.h"
#include "UserRepository.h"
#include <QString>
#include <QDateTime>
#include <memory>

/**
 * Authentication Service Interface
 */
class AuthenticationService {
public:
    virtual ~AuthenticationService() = default;
    virtual std::shared_ptr<User> authenticate(const QString& credential, const QString& password) = 0;
    virtual std::shared_ptr<User> registerUser(const QString& username, const QString& email,
                                               const QString& password, SubscriptionPackage pkg) = 0;
};

/**
 * Local Authentication Service
 */
class LocalAuthService : public AuthenticationService {
public:
    std::shared_ptr<User> authenticate(const QString& credential, const QString& password) override {
        return UserRepository::getInstance().findByUsernameAndPassword(credential, password);
    }

    std::shared_ptr<User> registerUser(const QString& username, const QString& email,
                                      const QString& password, SubscriptionPackage pkg) override {
        auto user = User::Builder()
            .userId(generateId())
            .username(username)
            .email(email)
            .subscriptionPackage(pkg)
            .authProvider(AuthProvider::LOCAL)
            .build();
        UserRepository::getInstance().save(user);
        return user;
    }

private:
    QString generateId() {
        return QString("USER_%1").arg(QDateTime::currentMSecsSinceEpoch());
    }
};

/**
 * Google OAuth Authentication Service
 */
class GoogleAuthService : public AuthenticationService {
public:
    std::shared_ptr<User> authenticate(const QString& credential, const QString& password) override {
        qDebug() << "Authenticating with Google...";
        return nullptr;
    }

    std::shared_ptr<User> registerUser(const QString& username, const QString& email,
                                      const QString& password, SubscriptionPackage pkg) override {
        auto user = User::Builder()
            .userId(generateId())
            .username(username)
            .email(email)
            .subscriptionPackage(pkg)
            .authProvider(AuthProvider::GOOGLE)
            .build();
        UserRepository::getInstance().save(user);
        return user;
    }

private:
    QString generateId() {
        return QString("GOOGLE_%1").arg(QDateTime::currentMSecsSinceEpoch());
    }
};

/**
 * Github OAuth Authentication Service
 */
class GithubAuthService : public AuthenticationService {
public:
    std::shared_ptr<User> authenticate(const QString& credential, const QString& password) override {
        qDebug() << "Authenticating with Github...";
        return nullptr;
    }

    std::shared_ptr<User> registerUser(const QString& username, const QString& email,
                                      const QString& password, SubscriptionPackage pkg) override {
        auto user = User::Builder()
            .userId(generateId())
            .username(username)
            .email(email)
            .subscriptionPackage(pkg)
            .authProvider(AuthProvider::GITHUB)
            .build();
        UserRepository::getInstance().save(user);
        return user;
    }

private:
    QString generateId() {
        return QString("GITHUB_%1").arg(QDateTime::currentMSecsSinceEpoch());
    }
};

#endif // AUTHENTICATION_SERVICE_H