#include "core/CourseManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#endif

static inline void ensureDirectoryExistsCourse(const std::string &path) {
#if defined(_WIN32)
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

CourseManager::CourseManager() {
    ensureDirectoryExistsCourse("Data");
    ensureDirectoryExistsCourse("Data/Contents");
    ensureDirectoryExistsCourse("Data/Quizzes");
    ensureDirectoryExistsCourse("Data/Certificates");
    loadFromFile();
    ensureSampleCourses();
}

void CourseManager::ensureSampleCourses() {
    if (!courses.empty()) return;

    // Course 1: C++ Basics
    Course c1("C001", "C++ Programming Basics", "Introductory course on C++", "Data/Quizzes/C001_quiz.txt");
    c1.addContent( Content("C001_1", "Introduction to C++", "text", "Data/Contents/C001_1.txt") );
    c1.addContent( Content("C001_2", "Variables and Data Types", "text", "Data/Contents/C001_2.txt") );
    c1.addContent( Content("C001_3", "Control Flow (if/loops)", "text", "Data/Contents/C001_3.txt") );

    // Course 2: Python Basics
    Course c2("C002", "Python for Beginners", "Basic Python syntax and usage", "Data/Quizzes/C002_quiz.txt");
    c2.addContent( Content("C002_1", "Python Introduction", "text", "Data/Contents/C002_1.txt") );
    c2.addContent( Content("C002_2", "Data Types in Python", "text", "Data/Contents/C002_2.txt") );
    c2.addContent( Content("C002_3", "Control Structures", "text", "Data/Contents/C002_3.txt") );

    // Course 3: DBMS Fundamentals
    Course c3("C003", "Database Fundamentals", "Basics of relational databases", "Data/Quizzes/C003_quiz.txt");
    c3.addContent( Content("C003_1", "Relational Model", "text", "Data/Contents/C003_1.txt") );
    c3.addContent( Content("C003_2", "SQL Intro", "text", "Data/Contents/C003_2.txt") );
    c3.addContent( Content("C003_3", "Normalization Basics", "text", "Data/Contents/C003_3.txt") );

    addCourse(c1);
    addCourse(c2);
    addCourse(c3);

    saveToFile();
}

void CourseManager::loadFromFile() {
    courses.clear();
    std::ifstream fin("Data/courses.dat");
    if (!fin.is_open()) return;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        Course c = Course::deserialize(line);
        courses[c.getId()] = c;
    }
    fin.close();
}

void CourseManager::saveToFile() {
    std::ofstream fout("Data/courses.dat");
    for (const auto& kv : courses) {
        fout << kv.second.serialize() << "\n";
    }
    fout.close();
}

std::vector<Course> CourseManager::listAllCourses() const {
    std::vector<Course> out;
    for (const auto& kv : courses) out.push_back(kv.second);
    return out;
}

bool CourseManager::hasCourse(const std::string& courseId) const {
    return courses.find(courseId) != courses.end();
}

Course CourseManager::getCourse(const std::string& courseId) const {
    auto it = courses.find(courseId);
    if (it != courses.end()) return it->second;
    return Course();
}

Course& CourseManager::getCourseRef(const std::string& courseId) {
    static Course emptyCourse;
    auto it = courses.find(courseId);
    if (it != courses.end()) return it->second;
    return emptyCourse;
}

void CourseManager::addCourse(const Course& c) {
    courses[c.getId()] = c;
    saveToFile();
}

bool CourseManager::removeCourse(const std::string& courseId) {
    auto it = courses.find(courseId);
    if (it != courses.end()) {
        courses.erase(it);
        saveToFile();
        return true;
    }
    return false;
}

bool CourseManager::addContentToCourse(const std::string& courseId, const Content& content) {
    auto it = courses.find(courseId);
    if (it != courses.end()) {
        it->second.addContent(content);
        saveToFile();
        return true;
    }
    return false;
}

// NEW: Delete course method
bool CourseManager::deleteCourse(const std::string& courseId) {
    return removeCourse(courseId); // Alias for removeCourse for clarity
}