#ifndef CERTIFICATE_H
#define CERTIFICATE_H
#ifdef _WIN32
#include <direct.h>
#endif

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>

static inline void ensureDirectoryExists(const std::string &path) {
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

class Certificate {
private:
    std::string userName;
    std::string courseName;
    std::string issueDate;

    std::string sanitizeFilename(const std::string &s) {
        std::string out = s;
        for (auto &c : out) if (c == ' ' || c == '/') c = '_';
        return out;
    }

public:
    Certificate(const std::string& u, const std::string& c) : userName(u), courseName(c) {
        std::time_t t = std::time(nullptr);
        issueDate = std::string(std::ctime(&t));
        // remove trailing newline
        if (!issueDate.empty() && issueDate.back() == '\n') issueDate.pop_back();
    }

    void generateTextCertificate() {
        ensureDirectoryExists("Data/Certificates");
        std::string fname = "Data/Certificates/" + sanitizeFilename(userName) + "_" + sanitizeFilename(courseName) + "_certificate.txt";
        std::ofstream fout(fname);
        fout << "============================================\n";
        fout << "            Certificate of Completion       \n";
        fout << "============================================\n\n";
        fout << "This is to certify that: " << userName << "\n\n";
        fout << "Has successfully completed the course: " << courseName << "\n\n";
        fout << "Date of Issue: " << issueDate << "\n\n";
        fout << "Congratulations!\n\n";
        fout << "--------------------------------------------\n";
        fout << "Object-Oriented LMS System\n";
        fout << "--------------------------------------------\n";
        fout.close();
        std::cout << "Certificate generated: " << fname << "\n";
    }
};

#endif // CERTIFICATE_H
