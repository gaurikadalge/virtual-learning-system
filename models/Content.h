#ifndef CONTENT_H
#define CONTENT_H

#include <string>
#include <iostream>
#include <fstream>

class Content {
private:
    std::string id;
    std::string title;
    std::string type; // "text" or "quiz"
    std::string filename; // Data/Contents/<filename>

public:
    Content() = default;
    Content(const std::string& _id, const std::string& _title,
            const std::string& _type, const std::string& _filename)
        : id(_id), title(_title), type(_type), filename(_filename) {}

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::string getType() const { return type; }
    std::string getFilename() const { return filename; }

    void displaySummary() const {
        std::cout << "[" << id << "] " << title << " (" << type << ")\n";
    }

    void displayFullContent() const {
        if (type != "text") {
            std::cout << "(No text content to display for this content item.)\n";
            return;
        }
        std::ifstream fin(filename);
        if (!fin.is_open()) {
            std::cout << "(Content file not found: " << filename << ")\n";
            return;
        }
        std::string line;
        std::cout << "----- " << title << " -----\n\n";
        while (std::getline(fin, line)) {
            std::cout << line << "\n";
        }
        std::cout << "\n----- End of Content -----\n";
        fin.close();
    }

    std::string serialize() const {
        // id|title|type|filename
        return id + "|" + title + "|" + type + "|" + filename;
    }

    static Content deserialize(const std::string& line) {
        std::string parts[4];
        size_t start = 0, pos;
        int i = 0;
        while (i < 3 && (pos = line.find('|', start)) != std::string::npos) {
            parts[i++] = line.substr(start, pos - start);
            start = pos + 1;
        }
        parts[3] = line.substr(start);
        return Content(parts[0], parts[1], parts[2], parts[3]);
    }
};

#endif // CONTENT_H
