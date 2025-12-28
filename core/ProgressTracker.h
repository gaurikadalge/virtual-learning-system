#ifndef PROGRESSTRACKER_H
#define PROGRESSTRACKER_H

#include <string>
#include <map>
#include <vector>
#include "models/Course.h"

class ProgressTracker {
private:
    // userId -> courseId -> vector of completed content ids
    std::map<std::string, std::map<std::string, std::vector<std::string>>> progress;
    // userId -> courseId -> quizPassed (0/1)
    std::map<std::string, std::map<std::string, bool>> quizPassed;

    void loadFromFile();
    void saveToFile();

public:
    ProgressTracker();
    void markContentComplete(const std::string& userId, const std::string& courseId, const std::string& contentId);
    bool hasCompletedContent(const std::string& userId, const std::string& courseId, const std::string& contentId) const;
    void setQuizPassed(const std::string& userId, const std::string& courseId, bool passed);
    bool isQuizPassed(const std::string& userId, const std::string& courseId) const;
    double getCourseProgressPercent(const std::string& userId, const Course& course) const;
    bool isCourseComplete(const std::string& userId, const Course& course) const;
};

#endif // PROGRESSTRACKER_H
