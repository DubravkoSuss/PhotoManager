#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "User.h"
#include "Logger_APP.h"
#include <QMap>
#include <QList>
#include <memory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include "MetricsCollector_APP.h"
#include <algorithm>

/**
 * Repository Pattern + Singleton for User data access
 */
class UserRepository {
public:
    static UserRepository& getInstance() {
        static UserRepository instance;
        return instance;
    }

    void save(std::shared_ptr<User> user) {
        users_[user->getUserId()] = user;
        Logger::getInstance().log("UserRepository", "User saved: " + user->getUsername());
        MetricsCollector::instance().incrementCounter("users.saved");
        saveRepo();
    }

    void deleteUser(const QString& userId) {
        if (users_.remove(userId)) {
            Logger::getInstance().log("UserRepository", "User deleted: " + userId);
            saveRepo();
        }
    }

    std::shared_ptr<User> findById(const QString& userId) {
        return users_.value(userId, nullptr);
    }

    std::shared_ptr<User> findByUsername(const QString& username) {
        auto users = users_.values();
        auto it = std::find_if(users.cbegin(), users.cend(),
                               [&username](const std::shared_ptr<User>& user) {
                                   return user->getUsername() == username;
                               });
        return it != users.cend() ? *it : nullptr;
    }

    std::shared_ptr<User> findByUsernameAndPassword(const QString& username, const QString& password) {
        auto user = findByUsername(username);
        if (user && user->getPasswordHash() == password) {
            return user;
        }
        return nullptr; // Invalid or wrong password
    }

    QList<std::shared_ptr<User>> findAll() {
        return users_.values();
    }

    // Delete copy constructor and assignment operator
    UserRepository(const UserRepository&) = delete;
    UserRepository& operator=(const UserRepository&) = delete;

private:
    UserRepository() {
        loadRepo();

        if (users_.isEmpty()) {
            // Create default admin
            auto admin = User::Builder()
                .userId("ADMIN_001")
                .username("admin")
                .password("admin123") // Default password
                .email("admin@photomanager.com")
                .userType(UserType::ADMINISTRATOR)
                .subscriptionPackage(SubscriptionPackage::GOLD)
                .build();
            users_[admin->getUserId()] = admin;
            saveRepo();
        }
    }

    void loadRepo() {
        QFile file("users.json");
        if (!file.open(QIODevice::ReadOnly)) return;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();

        for (const auto& val : array) {
            auto user = User::fromJson(val.toObject());
            users_[user->getUserId()] = user;
        }
    }

    void saveRepo() {
        QJsonArray array;
        for (const auto& user : users_) {
            array.append(user->toJson());
        }

        QJsonDocument doc(array);
        QFile file("users.json");
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
        }
    }

    QMap<QString, std::shared_ptr<User>> users_;
};

#endif // USER_REPOSITORY_H