#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <vector>
#include <sqlite3.h>

struct ChatMessage {
    std::string role;
    std::string content;
};

class MemoryManager {
private:
    sqlite3 *db = nullptr;
    std::string dbPath;

public:
    MemoryManager(const std::string &path);
    ~MemoryManager();

    bool init();
    void saveFact(const std::string &key, const std::string &value);
    std::string getFactsFormatted();
    void addMessage(const std::string &role, const std::string &content);
    std::vector<ChatMessage> getRecentHistory(int limit = 6);
};

#endif // MEMORY_H
