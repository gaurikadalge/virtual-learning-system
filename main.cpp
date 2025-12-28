#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <fstream>

#include "core/UserManager.h"
#include "core/CourseManager.h"
#include "core/ProgressTracker.h"
#include "models/Quiz.h"
#include "models/Certificate.h"

void pause() {
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper function to create content file
bool createContentFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << content;
    file.close();
    return true;
}

int main() {
    UserManager userManager;
    CourseManager courseManager;
    ProgressTracker progress;

    std::cout << "====== Object-Oriented LMS (Console) ======\n\n";

    bool running = true;
    std::string currentUserId;
    std::string currentUserName;
    std::string currentUserRole;

    while (running) {
        if (currentUserId.empty()) {
            std::cout << "\n1) Register\n2) Login\n3) Exit\nChoose: ";
            int c; std::cin >> c;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (c == 1) {
                std::string name, email, pass, role;
                std::cout << "Name: "; std::getline(std::cin, name);
                std::cout << "Email: "; std::getline(std::cin, email);
                std::cout << "Password: "; std::getline(std::cin, pass);
                std::cout << "Role (student/instructor): "; std::getline(std::cin, role);
                std::string uid;
                if (userManager.registerUser(name, email, pass, role, uid)) {
                    std::cout << "Registered. Your user id: " << uid << "\n";
                } else {
                    std::cout << "Email already exists. Try login.\n";
                }
            } else if (c == 2) {
                std::string email, pass;
                std::cout << "Email: "; std::getline(std::cin, email);
                std::cout << "Password: "; std::getline(std::cin, pass);
                User u;
                if (userManager.loginUser(email, pass, u)) {
                    currentUserId = u.getId();
                    currentUserName = u.getName();
                    currentUserRole = u.getRole();
                    std::cout << "Welcome, " << currentUserName << " (" << currentUserRole << ")\n";
                } else {
                    std::cout << "Login failed.\n";
                }
            } else {
                running = false;
            }
        } else {
            // ROLE-BASED MENUS
            if (currentUserRole == "instructor") {
                // INSTRUCTOR DASHBOARD
                std::cout << "\n=== INSTRUCTOR DASHBOARD ===\n";
                std::cout << "1) Create New Course\n";
                std::cout << "2) List All Courses\n";
                std::cout << "3) View Course Details\n";
                std::cout << "4) Add Content to Course\n";
                std::cout << "5) Delete Course\n"; // NEW: Delete course option
                std::cout << "6) View Student Progress\n";
                std::cout << "7) Logout\n";
                std::cout << "Choose: ";
                int c; std::cin >> c;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (c == 1) {
                    // CREATE NEW COURSE
                    std::string courseId, title, description, quizFile;
                    std::cout << "Enter Course ID (e.g., C004): ";
                    std::getline(std::cin, courseId);
                    std::cout << "Enter Course Title: ";
                    std::getline(std::cin, title);
                    std::cout << "Enter Course Description: ";
                    std::getline(std::cin, description);
                    std::cout << "Enter Quiz File (or leave empty): ";
                    std::getline(std::cin, quizFile);
                    
                    Course newCourse(courseId, title, description, quizFile);
                    courseManager.addCourse(newCourse);
                    std::cout << "✅ Course created successfully!\n";
                    
                } else if (c == 2) {
                    // LIST ALL COURSES
                    auto courses = courseManager.listAllCourses();
                    std::cout << "\n📚 All Courses (" << courses.size() << "):\n";
                    for (const auto& course : courses) {
                        course.displaySummary();
                    }
                    
                } else if (c == 3) {
                    // VIEW COURSE DETAILS
                    std::string courseId;
                    std::cout << "Enter course id: ";
                    std::getline(std::cin, courseId);
                    if (!courseManager.hasCourse(courseId)) {
                        std::cout << "❌ Course not found.\n";
                    } else {
                        Course course = courseManager.getCourse(courseId);
                        std::cout << "\n=== COURSE DETAILS ===\n";
                        course.displaySummary();
                        const auto& contents = course.getContents();
                        std::cout << "\nCourse Contents (" << contents.size() << " items):\n";
                        for (size_t i = 0; i < contents.size(); ++i) {
                            std::cout << i+1 << ") ";
                            contents[i].displaySummary();
                        }
                        if (!course.getQuizFile().empty()) {
                            std::cout << "📝 Quiz: " << course.getQuizFile() << "\n";
                        }
                    }
                    
                } else if (c == 4) {
                    // ADD CONTENT TO COURSE - IMPROVED VERSION
                    std::string courseId;
                    std::cout << "Enter course id to add content: ";
                    std::getline(std::cin, courseId);
                    if (!courseManager.hasCourse(courseId)) {
                        std::cout << "❌ Course not found.\n";
                    } else {
                        Course course = courseManager.getCourse(courseId);
                        std::cout << "Adding content to: " << course.getTitle() << "\n";
                        
                        std::string contentId, title, type, contentText;
                        std::cout << "Enter Content ID (e.g., " << courseId << "_4): ";
                        std::getline(std::cin, contentId);
                        std::cout << "Enter Content Title: ";
                        std::getline(std::cin, title);
                        std::cout << "Enter Content Type (text/quiz): ";
                        std::getline(std::cin, type);
                        
                        if (type == "text") {
                            std::cout << "Enter the content text (press Enter on empty line to finish):\n";
                            std::string line;
                            while (true) {
                                std::getline(std::cin, line);
                                if (line.empty()) break;
                                contentText += line + "\n";
                            }
                            
                            // Create file path
                            std::string filename = "Data/Contents/" + contentId + ".txt";
                            
                            // Create the content file
                            if (createContentFile(filename, contentText)) {
                                Content newContent(contentId, title, type, filename);
                                if (courseManager.addContentToCourse(courseId, newContent)) {
                                    std::cout << "✅ Content added successfully!\n";
                                    std::cout << "📁 File created: " << filename << "\n";
                                } else {
                                    std::cout << "❌ Failed to add content to course.\n";
                                }
                            } else {
                                std::cout << "❌ Failed to create content file.\n";
                            }
                        } else {
                            std::cout << "❌ Only 'text' content type is supported for automatic creation.\n";
                        }
                    }
                    
                } else if (c == 5) {
                    // DELETE COURSE - NEW FEATURE
                    std::string courseId;
                    std::cout << "Enter course id to delete: ";
                    std::getline(std::cin, courseId);
                    
                    if (!courseManager.hasCourse(courseId)) {
                        std::cout << "❌ Course not found.\n";
                    } else {
                        Course course = courseManager.getCourse(courseId);
                        std::cout << "You are about to delete: " << course.getTitle() << "\n";
                        std::cout << "This will remove the course and all its content from the system.\n";
                        std::cout << "Type 'DELETE' to confirm: ";
                        std::string confirmation;
                        std::getline(std::cin, confirmation);
                        
                        if (confirmation == "DELETE") {
                            if (courseManager.deleteCourse(courseId)) {
                                std::cout << "✅ Course deleted successfully!\n";
                            } else {
                                std::cout << "❌ Failed to delete course.\n";
                            }
                        } else {
                            std::cout << "❌ Deletion cancelled.\n";
                        }
                    }
                    
                } else if (c == 6) {
                    // VIEW STUDENT PROGRESS
                    std::cout << "\n=== STUDENT PROGRESS OVERVIEW ===\n";
                    
                    auto courses = courseManager.listAllCourses();
                    auto students = userManager.getUsersByRole("student");
                    
                    if (students.empty()) {
                        std::cout << "No students found in the system.\n";
                    } else {
                        std::cout << "📊 Found " << students.size() << " student(s) in system:\n";
                        for (const auto& student : students) {
                            std::cout << " - " << student.getName() << " (" << student.getEmail() << ")\n";
                        }
                        std::cout << "\n";
                        
                        for (const auto& course : courses) {
                            std::cout << "🎯 Course: " << course.getTitle() << " [" << course.getId() << "]\n";
                            std::cout << "----------------------------------------\n";
                            
                            int completedCount = 0;
                            int totalStudents = 0;
                            
                            for (const auto& student : students) {
                                double progressPercent = progress.getCourseProgressPercent(student.getId(), course);
                                bool isComplete = progress.isCourseComplete(student.getId(), course);
                                
                                if (progressPercent > 0) {
                                    totalStudents++;
                                    if (isComplete) completedCount++;
                                    
                                    std::cout << "👤 " << student.getName() << ":\n";
                                    std::cout << "   Progress: " << progressPercent << "%";
                                    std::cout << " | Status: " << (isComplete ? "✅ COMPLETED" : "📖 IN PROGRESS") << "\n";
                                    
                                    const auto& contents = course.getContents();
                                    int lessonsCompleted = 0;
                                    for (const auto& content : contents) {
                                        if (progress.hasCompletedContent(student.getId(), course.getId(), content.getId())) {
                                            lessonsCompleted++;
                                        }
                                    }
                                    std::cout << "   Lessons: " << lessonsCompleted << "/" << contents.size() << " completed";
                                    
                                    if (!course.getQuizFile().empty()) {
                                        bool quizPassed = progress.isQuizPassed(student.getId(), course.getId());
                                        std::cout << " | Quiz: " << (quizPassed ? "✅ PASSED" : "❌ NOT TAKEN");
                                    }
                                    std::cout << "\n\n";
                                }
                            }
                            
                            if (totalStudents > 0) {
                                double completionRate = (completedCount * 100.0) / totalStudents;
                                std::cout << "📈 COURSE STATISTICS:\n";
                                std::cout << "   Enrolled Students: " << totalStudents << "\n";
                                std::cout << "   Completed: " << completedCount << "\n";
                                std::cout << "   Completion Rate: " << completionRate << "%\n";
                            } else {
                                std::cout << "No students have started this course yet.\n";
                            }
                            std::cout << "\n";
                        }
                    }
                    
                } else if (c == 7) {
                    currentUserId.clear();
                    currentUserName.clear();
                    currentUserRole.clear();
                    std::cout << "Logged out.\n";
                } else {
                    std::cout << "❌ Invalid option.\n";
                }
                pause();
                
            } else {
                // STUDENT MENU (existing functionality - unchanged)
                std::cout << "\n1) List Courses\n2) View Course Details\n3) View Progress\n4) Logout\nChoose: ";
                int c; std::cin >> c;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                if (c == 1) {
                    auto courses = courseManager.listAllCourses();
                    std::cout << "\nAvailable Courses:\n";
                    for (const auto& course : courses) course.displaySummary();
                } else if (c == 2) {
                    std::string courseId;
                    std::cout << "Enter course id (e.g. C001): ";
                    std::getline(std::cin, courseId);
                    if (!courseManager.hasCourse(courseId)) {
                        std::cout << "Course not found.\n";
                    } else {
                        Course course = courseManager.getCourse(courseId);
                        course.displaySummary();
                        const auto& contents = course.getContents();
                        std::cout << "\nContents:\n";
                        for (size_t i = 0; i < contents.size(); ++i) {
                            std::cout << i+1 << ") ";
                            contents[i].displaySummary();
                            bool done = progress.hasCompletedContent(currentUserId, courseId, contents[i].getId());
                            std::cout << (done ? " [COMPLETED]" : " [NOT DONE]") << "\n";
                        }

                        std::cout << "\nOptions:\n1) Read content\n2) Take quiz\n3) Back\nChoose: ";
                        int opt; std::cin >> opt;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        if (opt == 1) {
                            std::cout << "Enter content number: ";
                            int ci; std::cin >> ci;
                            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                            if (ci < 1 || ci > (int)contents.size()) {
                                std::cout << "Invalid.\n";
                            } else {
                                const auto& cont = contents[ci-1];
                                cont.displayFullContent();
                                progress.markContentComplete(currentUserId, courseId, cont.getId());
                                std::cout << "Marked as completed.\n";

                                double p = progress.getCourseProgressPercent(currentUserId, course);
                                std::cout << "Current progress: " << p << "%\n";
                                if (progress.isCourseComplete(currentUserId, course)) {
                                    std::cout << "Course complete! Generating certificate...\n";
                                    Certificate cert(currentUserName, course.getTitle());
                                    cert.generateTextCertificate();
                                }
                            }
                            pause();
                        } else if (opt == 2) {
                            std::string qfile = course.getQuizFile();
                            if (qfile.empty()) {
                                std::cout << "No quiz for this course.\n";
                            } else {
                                Quiz quiz;
                                if (!quiz.loadFromFile(qfile)) {
                                    std::cout << "Quiz file not found or empty.\n";
                                } else {
                                    int score = quiz.startInteractive();
                                    bool passed = score >= 60;
                                    progress.setQuizPassed(currentUserId, courseId, passed);
                                    std::cout << (passed ? "Quiz passed.\n" : "Quiz failed.\n");
                                    double p = progress.getCourseProgressPercent(currentUserId, course);
                                    std::cout << "Current progress: " << p << "%\n";
                                    if (progress.isCourseComplete(currentUserId, course)) {
                                        std::cout << "Course complete! Generating certificate...\n";
                                        Certificate cert(currentUserName, course.getTitle());
                                        cert.generateTextCertificate();
                                    }
                                }
                            }
                            pause();
                        }
                    }
                } else if (c == 3) {
                    std::string courseId;
                    std::cout << "Enter course id to view progress (or ALL): ";
                    std::getline(std::cin, courseId);
                    if (courseId == "ALL" || courseId == "all") {
                        auto courses = courseManager.listAllCourses();
                        for (const auto& course : courses) {
                            double p = progress.getCourseProgressPercent(currentUserId, course);
                            std::cout << course.getId() << " - " << course.getTitle() << " : " << p << "%\n";
                        }
                    } else {
                        if (!courseManager.hasCourse(courseId)) {
                            std::cout << "Course not found.\n";
                        } else {
                            Course course = courseManager.getCourse(courseId);
                            double p = progress.getCourseProgressPercent(currentUserId, course);
                            std::cout << "Progress for " << course.getTitle() << " = " << p << "%\n";
                        }
                    }
                    pause();
                } else if (c == 4) {
                    currentUserId.clear();
                    currentUserName.clear();
                    currentUserRole.clear();
                    std::cout << "Logged out.\n";
                } else {
                    std::cout << "Invalid option.\n";
                }
            }
        }
    }

    std::cout << "Exiting LMS. Goodbye.\n";
    return 0;
}