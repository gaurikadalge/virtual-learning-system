#ifndef COURSEMANAGER_H
#define COURSEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "models/Course.h"

class CourseManager {
private:
    std::map<std::string, Course> courses; // courseId -> Course

    void loadFromFile();
    void saveToFile();

public:
    CourseManager();
    void ensureSampleCourses();
    std::vector<Course> listAllCourses() const;
    bool hasCourse(const std::string& courseId) const;
    Course getCourse(const std::string& courseId) const;
    void addCourse(const Course& c);
    bool removeCourse(const std::string& courseId);
    Course& getCourseRef(const std::string& courseId);
    bool addContentToCourse(const std::string& courseId, const Content& content);
    bool deleteCourse(const std::string& courseId); // NEW: Delete course method
};

#endif // COURSEMANAGER_H