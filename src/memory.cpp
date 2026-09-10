#include "memory.h"
#include <iostream>

MemoryManager::MemoryManager(const std::string &path) : dbPath(path) {
    init();
}

MemoryManager::~MemoryManager() {
    if (db) {
        sqlite3_close(db);
    }
}

bool MemoryManager::init() {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        return false;
    }

    const char *createFactsTable = 
        "CREATE TABLE IF NOT EXISTS facts ("
        "key TEXT PRIMARY KEY, "
        "value TEXT NOT NULL);";

    const char *createHistoryTable = 
        "CREATE TABLE IF NOT EXISTS history ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "role TEXT NOT NULL, "
        "content TEXT NOT NULL, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    sqlite3_exec(db, createFactsTable, nullptr, nullptr, nullptr);
    sqlite3_exec(db, createHistoryTable, nullptr, nullptr, nullptr);
    return true;
}

void MemoryManager::saveFact(const std::string &key, const std::string &value) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR REPLACE INTO facts (key, value) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

std::string MemoryManager::getFactsFormatted() {
    std::string facts = "";
    const char *sql = "SELECT key, value FROM facts;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string key = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            facts += "- " + key + ": " + val + "\n";
        }
    }
    sqlite3_finalize(stmt);
    return facts.empty() ? "- User's Name: Nicho\n" : facts;
}

void MemoryManager::addMessage(const std::string &role, const std::string &content) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO history (role, content) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, role.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

std::vector<ChatMessage> MemoryManager::getRecentHistory(int limit) {
    std::vector<ChatMessage> history;
    std::string sql = "SELECT role, content FROM (SELECT id, role, content FROM history ORDER BY id DESC LIMIT " 
                      + std::to_string(limit) + ") ORDER BY id ASC;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            history.push_back({role, text});
        }
    }
    sqlite3_finalize(stmt);
    return history;
}
