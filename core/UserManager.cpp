#include "core/UserManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#endif

static inline void ensureDirectoryExistsCore(const std::string &path) {
#if defined(_WIN32)
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

UserManager::UserManager() {
    ensureDirectoryExistsCore("Data");
    loadFromFile();
    ensureSampleUsers();
}

void UserManager::ensureSampleUsers() {
    // If no users exist, create a default instructor and a student
    if (usersById.empty()) {
        std::string id1 = "U001";
        usersById[id1] = User(id1, "Instructor One", "instructor@example.com", "instrpass", "instructor");
        emailToId["instructor@example.com"] = id1;
        std::string id2 = "U002";
        usersById[id2] = User(id2, "Student One", "student@example.com", "studpass", "student");
        emailToId["student@example.com"] = id2;
        saveToFile();
    }
}

void UserManager::loadFromFile() {
    usersById.clear();
    emailToId.clear();
    std::ifstream fin("Data/users.dat");
    if (!fin.is_open()) return;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.size() == 0) continue;
        User u = User::deserialize(line);
        usersById[u.getId()] = u;
        emailToId[u.getEmail()] = u.getId();
    }
    fin.close();
}

void UserManager::saveToFile() {
    std::ofstream fout("Data/users.dat");
    for (const auto& kv : usersById) {
        fout << kv.second.serialize() << "\n";
    }
    fout.close();
}

bool UserManager::existsEmail(const std::string& email) const {
    return emailToId.find(email) != emailToId.end();
}

bool UserManager::registerUser(const std::string& name, const std::string& email,
                               const std::string& password, const std::string& role, std::string& outUserId) {
    if (existsEmail(email)) return false;
    // create simple incremental id
    int maxId = 0;
    for (const auto& kv : usersById) {
        std::string id = kv.first;
        if (id.size() >= 4 && id[0] == 'U') {
            int num = std::stoi(id.substr(1));
            if (num > maxId) maxId = num;
        }
    }
    int next = maxId + 1;
    std::ostringstream oss;
    oss << "U" << std::setw(3) << std::setfill('0') << next;
    std::string id = oss.str();
    User u(id, name, email, password, role);
    usersById[id] = u;
    emailToId[email] = id;
    saveToFile();
    outUserId = id;
    return true;
}

bool UserManager::loginUser(const std::string& email, const std::string& password, User& outUser) {
    auto it = emailToId.find(email);
    if (it == emailToId.end()) return false;
    std::string id = it->second;
    auto it2 = usersById.find(id);
    if (it2 == usersById.end()) return false;
    if (it2->second.getPassword() != password) return false;
    outUser = it2->second;
    return true;
}

User UserManager::getUserById(const std::string& id) {
    auto it = usersById.find(id);
    if (it != usersById.end()) return it->second;
    return User();
}

std::vector<User> UserManager::getAllUsers() const {
    std::vector<User> users;
    for (const auto& pair : usersById) {
        users.push_back(pair.second);
    }
    return users;
}

std::vector<User> UserManager::getUsersByRole(const std::string& role) const {
    std::vector<User> filteredUsers;
    for (const auto& pair : usersById) {
        if (pair.second.getRole() == role) {
            filteredUsers.push_back(pair.second);
        }
    }
    return filteredUsers;
}