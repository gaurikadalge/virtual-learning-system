#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct QuizQuestion {
    std::string question;
    std::string a, b, c, d;
    char answer; // 'a','b','c','d'
};

class Quiz {
private:
    std::vector<QuizQuestion> questions;

public:
    bool loadFromFile(const std::string& filename) {
        questions.clear();
        std::ifstream fin(filename);
        if (!fin.is_open()) return false;
        std::string line;
        while (std::getline(fin, line)) {
            if (line.size() == 0) continue;
            // format: question|a|b|c|d|answer
            std::stringstream ss(line);
            QuizQuestion q;
            std::getline(ss, q.question, '|');
            std::getline(ss, q.a, '|');
            std::getline(ss, q.b, '|');
            std::getline(ss, q.c, '|');
            std::getline(ss, q.d, '|');
            std::string ans;
            std::getline(ss, ans);
            q.answer = ans.empty() ? 'a' : ans[0];
            questions.push_back(q);
        }
        fin.close();
        return !questions.empty();
    }

    // Returns score percentage [0..100]
    int startInteractive() {
        if (questions.empty()) return 0;
        int correct = 0;
        int idx = 1;
        for (const auto& q : questions) {
            std::cout << "\nQ" << idx++ << ": " << q.question << "\n";
            std::cout << "a) " << q.a << "\n";
            std::cout << "b) " << q.b << "\n";
            std::cout << "c) " << q.c << "\n";
            std::cout << "d) " << q.d << "\n";
            char resp;
            std::cout << "Your answer (a/b/c/d): ";
            std::cin >> resp;
            resp = tolower(resp);
            if (resp == q.answer) {
                std::cout << "Correct!\n";
                ++correct;
            } else {
                std::cout << "Wrong. Correct answer: " << q.answer << "\n";
            }
        }
        int score = (int)((double)correct / questions.size() * 100.0 + 0.5);
        std::cout << "\nQuiz finished. Score: " << score << "% (" << correct << "/" << questions.size() << ")\n";
        return score;
    }
};

#endif // QUIZ_H
