#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User {
private:
    std::string id;
    std::string name;
    std::string email;
    std::string password;
    std::string role; // "student" or "instructor"

public:
    User() = default;
    User(const std::string& _id, const std::string& _name, const std::string& _email,
         const std::string& _password, const std::string& _role)
        : id(_id), name(_name), email(_email), password(_password), role(_role) {}

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPassword() const { return password; }
    std::string getRole() const { return role; }

    void setPassword(const std::string& p) { password = p; }

    std::string serialize() const {
        // id|name|email|password|role
        return id + "|" + name + "|" + email + "|" + password + "|" + role;
    }

    static User deserialize(const std::string& line) {
        std::string parts[5];
        size_t start = 0, pos;
        int i = 0;
        while (i < 4 && (pos = line.find('|', start)) != std::string::npos) {
            parts[i++] = line.substr(start, pos - start);
            start = pos + 1;
        }
        parts[4] = line.substr(start);
        return User(parts[0], parts[1], parts[2], parts[3], parts[4]);
    }
};

#endif // USER_H
