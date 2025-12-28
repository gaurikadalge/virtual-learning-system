#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>
#include "Content.h"

class Course {
private:
    std::string id;
    std::string title;
    std::string description;
    std::vector<Content> contents;
    std::string quizFile; // Data/Quizzes/<file> or empty

public:
    Course() = default;
    Course(const std::string& _id, const std::string& _title,
           const std::string& _description, const std::string& _quizFile = "")
        : id(_id), title(_title), description(_description), quizFile(_quizFile) {}

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }
    std::string getQuizFile() const { return quizFile; }

    void addContent(const Content& c) { contents.push_back(c); }
    const std::vector<Content>& getContents() const { return contents; }

    void displaySummary() const {
        std::cout << "[" << id << "] " << title << " - " << description << "\n";
    }

    std::string serialize() const {
        // id|title|description|quizFile|contentIdList (content serialized separately)
        std::string line = id + "|" + title + "|" + description + "|" + quizFile + "|";
        for (size_t i = 0; i < contents.size(); ++i) {
            line += contents[i].serialize();
            if (i + 1 < contents.size()) line += ";;"; // separator between contents
        }
        return line;
    }

    static Course deserialize(const std::string& line) {
        // id|title|description|quizFile|content1;;content2;;...
        std::string parts[5];
        size_t start = 0, pos;
        int i = 0;
        while (i < 4 && (pos = line.find('|', start)) != std::string::npos) {
            parts[i++] = line.substr(start, pos - start);
            start = pos + 1;
        }
        parts[4] = line.substr(start);

        Course course(parts[0], parts[1], parts[2], parts[3]);

        // parse contents
        std::string rest = parts[4];
        size_t cur = 0;
        while (cur < rest.size()) {
            size_t sep = rest.find(";;", cur);
            std::string cstr;
            if (sep == std::string::npos) {
                cstr = rest.substr(cur);
                cur = rest.size();
            } else {
                cstr = rest.substr(cur, sep - cur);
                cur = sep + 2;
            }
            if (!cstr.empty()) {
                Content c = Content::deserialize(cstr);
                course.addContent(c);
            }
        }
        return course;
    }
};

#endif // COURSE_H
