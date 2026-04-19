#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "User.h"
#include "Logger.h"
#include <QMap>
#include <QList>
#include <memory>

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
    }

    std::shared_ptr<User> findById(const QString& userId) {
        return users_.value(userId, nullptr);
    }

    std::shared_ptr<User> findByUsername(const QString& username) {
        for (auto user : users_.values()) {
            if (user->getUsername() == username) {
                return user;
            }
        }
        return nullptr;
    }

    std::shared_ptr<User> findByUsernameAndPassword(const QString& username, const QString& password) {
        // Simplified - in real app, hash and compare passwords
        return findByUsername(username);
    }

    QList<std::shared_ptr<User>> findAll() {
        return users_.values();
    }

    // Delete copy constructor and assignment operator
    UserRepository(const UserRepository&) = delete;
    UserRepository& operator=(const UserRepository&) = delete;

private:
    UserRepository() {
        // Create default admin
        auto admin = User::Builder()
            .userId("ADMIN_001")
            .username("admin")
            .email("admin@photomanager.com")
            .userType(UserType::ADMINISTRATOR)
            .subscriptionPackage(SubscriptionPackage::GOLD)
            .build();
        users_[admin->getUserId()] = admin;
    }

    QMap<QString, std::shared_ptr<User>> users_;
};

#endif // USER_REPOSITORY_H