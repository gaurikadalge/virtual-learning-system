#include "core/ProgressTracker.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>
#endif

static inline void ensureDirectoryExistsProgress(const std::string &path) {
#if defined(_WIN32)
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

ProgressTracker::ProgressTracker() {
    ensureDirectoryExistsProgress("Data");
    loadFromFile();
}

void ProgressTracker::loadFromFile() {
    progress.clear();
    quizPassed.clear();
    std::ifstream fin("Data/progress.dat");
    if (!fin.is_open()) return;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        // userId|courseId|contentId1,contentId2,...|quizPassed
        std::stringstream ss(line);
        std::string userId, courseId, contentList, quizFlag;
        std::getline(ss, userId, '|');
        std::getline(ss, courseId, '|');
        std::getline(ss, contentList, '|');
        std::getline(ss, quizFlag);
        if (!contentList.empty()) {
            std::stringstream sc(contentList);
            std::string cid;
            while (std::getline(sc, cid, ',')) {
                if (!cid.empty())
                    progress[userId][courseId].push_back(cid);
            }
        }
        quizPassed[userId][courseId] = (quizFlag == "1");
    }
    fin.close();
}

void ProgressTracker::saveToFile() {
    std::ofstream fout("Data/progress.dat");
    for (const auto& pu : progress) {
        const std::string& userId = pu.first;
        for (const auto& pc : pu.second) {
            const std::string& courseId = pc.first;
            const auto& vec = pc.second;
            std::string contentList;
            for (size_t i = 0; i < vec.size(); ++i) {
                contentList += vec[i];
                if (i + 1 < vec.size()) contentList += ",";
            }
            bool q = false;
            auto it = quizPassed.find(userId);
            if (it != quizPassed.end()) {
                auto it2 = it->second.find(courseId);
                if (it2 != it->second.end()) q = it2->second;
            }
            fout << userId << "|" << courseId << "|" << contentList << "|" << (q ? "1" : "0") << "\n";
        }
    }
    fout.close();
}

void ProgressTracker::markContentComplete(const std::string& userId, const std::string& courseId, const std::string& contentId) {
    auto &vec = progress[userId][courseId];
    if (std::find(vec.begin(), vec.end(), contentId) == vec.end()) {
        vec.push_back(contentId);
        saveToFile();
    }
}

bool ProgressTracker::hasCompletedContent(const std::string& userId, const std::string& courseId, const std::string& contentId) const {
    auto it = progress.find(userId);
    if (it == progress.end()) return false;
    auto it2 = it->second.find(courseId);
    if (it2 == it->second.end()) return false;
    return std::find(it2->second.begin(), it2->second.end(), contentId) != it2->second.end();
}

void ProgressTracker::setQuizPassed(const std::string& userId, const std::string& courseId, bool passed) {
    quizPassed[userId][courseId] = passed;
    saveToFile();
}

bool ProgressTracker::isQuizPassed(const std::string& userId, const std::string& courseId) const {
    auto it = quizPassed.find(userId);
    if (it == quizPassed.end()) return false;
    auto it2 = it->second.find(courseId);
    if (it2 == it->second.end()) return false;
    return it2->second;
}

double ProgressTracker::getCourseProgressPercent(const std::string& userId, const Course& course) const {
    const auto& contents = course.getContents();
    if (contents.empty()) return 0.0;
    size_t total = contents.size();
    size_t completed = 0;
    auto it = progress.find(userId);
    if (it != progress.end()) {
        auto it2 = it->second.find(course.getId());
        if (it2 != it->second.end()) {
            for (const auto& cid : it2->second) {
                // ensure content belongs to course
                for (const auto& c : contents) {
                    if (c.getId() == cid) {
                        ++completed;
                        break;
                    }
                }
            }
        }
    }
    double contentPercent = ((double)completed / (double)total) * 100.0;
    // weight quiz as 20% if a quiz exists for the course
    double quizWeight = (course.getQuizFile().empty() ? 0.0 : 20.0);
    double contentWeight = 100.0 - quizWeight;
    double part = (contentPercent / 100.0) * contentWeight;
    double quizPart = isQuizPassed(userId, course.getId()) ? quizWeight : 0.0;
    double totalPercent = part + quizPart;
    if (totalPercent > 100.0) totalPercent = 100.0;
    return totalPercent;
}

bool ProgressTracker::isCourseComplete(const std::string& userId, const Course& course) const {
    return getCourseProgressPercent(userId, course) >= 100.0 - 1e-9;
}
