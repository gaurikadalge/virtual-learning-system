#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "models/User.h"

class UserManager {
private:
    std::map<std::string, User> usersById;
    std::map<std::string, std::string> emailToId; // email -> id

    void loadFromFile();
    void saveToFile();

public:
    UserManager();
    bool registerUser(const std::string& name, const std::string& email,
                      const std::string& password, const std::string& role, std::string& outUserId);
    bool loginUser(const std::string& email, const std::string& password, User& outUser);
    User getUserById(const std::string& id);
    bool existsEmail(const std::string& email) const;
    void ensureSampleUsers();
    std::vector<User> getAllUsers() const; // NEW: Get all users
    std::vector<User> getUsersByRole(const std::string& role) const; // NEW: Get users by role
};

#endif // USERMANAGER_H