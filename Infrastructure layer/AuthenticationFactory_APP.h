#ifndef AUTHENTICATION_FACTORY_H
#define AUTHENTICATION_FACTORY_H

#include "AuthenticationService_APP.h"
#include <memory>

/**
 * Factory Pattern for Authentication Services
 */
class AuthenticationFactory {
public:
    static std::unique_ptr<AuthenticationService> getAuthService(AuthProvider provider) {
        switch (provider) {
            case AuthProvider::LOCAL:
                return std::make_unique<LocalAuthService>();
            case AuthProvider::GOOGLE:
                return std::make_unique<GoogleAuthService>();
            case AuthProvider::GITHUB:
                return std::make_unique<GithubAuthService>();
            default:
                return std::make_unique<LocalAuthService>();
        }
    }
};

#endif // AUTHENTICATION_FACTORY_H