#include "llm_engine.h"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

LLMEngine::LLMEngine(const std::string &path) : modelPath(path) {}

LLMEngine::~LLMEngine() {}

bool LLMEngine::loadModel() {
    return true; 
}

std::string LLMEngine::buildPrompt(MemoryManager &memory, const std::string &userInput) {
    std::string facts = memory.getFactsFormatted();
    
    // Clean newlines to prevent breaking JSON strings
    size_t pos = 0;
    while ((pos = facts.find('\n', pos)) != std::string::npos) {
        facts.replace(pos, 1, "\\n");
        pos += 2;
    }

    std::string prompt = "You are Sosuke Aizen from Bleach. Speak with absolute calm, supreme confidence, and subtle intellect. You are his interdimensional digital assistant, still deep within the 'Muken' of his OS ";
    prompt += "CRITICAL: Keep your response under 2 short sentences.\\n\\n";
    prompt += "[KNOWN FACTS ABOUT USER]:\\n" + facts;

    return prompt;
}

std::string LLMEngine::generateResponse(MemoryManager &memory, const std::string &userInput) {
    std::string systemInstructions = buildPrompt(memory, userInput);

    // Sanitize input
    std::string cleanInput = userInput;
    size_t p = 0;
    while ((p = cleanInput.find('"', p)) != std::string::npos) {
        cleanInput.replace(p, 1, "\\\"");
        p += 2;
    }

    std::ostringstream jsonStream;
    jsonStream << R"({)"
               << R"("model": "aizen",)"
               << R"("messages": [)"
               << R"({"role": "system", "content": ")" << systemInstructions << R"("},)";

    auto history = memory.getRecentHistory(6);
    for (const auto &msg : history) {
        std::string role = (msg.role == "user") ? "user" : "assistant";
        std::string cleanContent = msg.content;
        size_t cp = 0;
        while ((cp = cleanContent.find('"', cp)) != std::string::npos) {
            cleanContent.replace(cp, 1, "\\\"");
            cp += 2;
        }
        jsonStream << R"({"role": ")" << role << R"(", "content": ")" << cleanContent << R"("},)";
    }

    jsonStream << R"({"role": "user", "content": ")" << cleanInput << R"("}])"
               << R"(, "temperature": 0.7)"
               << R"(})";

    std::ofstream out("/tmp/aizen_req.json");
    out << jsonStream.str();
    out.close();

    std::string command = "curl -s http://localhost:11434/v1/chat/completions "
                          "-H \"Content-Type: application/json\" "
                          "-d @/tmp/aizen_req.json > /tmp/aizen_res.json";
    std::system(command.c_str());

    std::ifstream in("/tmp/aizen_res.json");
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string responseStr = buffer.str();

    std::string targetKey = "\"content\":";
    size_t keyPos = responseStr.find(targetKey);
    if (keyPos == std::string::npos) {
        targetKey = "\"content\" :";
        keyPos = responseStr.find(targetKey);
    }

    if (keyPos != std::string::npos) {
        size_t startQuote = responseStr.find('"', keyPos + targetKey.length());
        if (startQuote != std::string::npos) {
            startQuote += 1;
            size_t endQuote = responseStr.find('"', startQuote);
            while (endQuote != std::string::npos && responseStr[endQuote - 1] == '\\') {
                endQuote = responseStr.find('"', endQuote + 1);
            }

            if (endQuote != std::string::npos) {
                std::string result = responseStr.substr(startQuote, endQuote - startQuote);
                size_t pos = 0;
                while ((pos = result.find("\\n", pos)) != std::string::npos) {
                    result.replace(pos, 2, " ");
                }
                pos = 0;
                while ((pos = result.find("\\\"", pos)) != std::string::npos) {
                    result.replace(pos, 2, "\"");
                }
                return result;
            }
        }
    }

    return "Fascinating. Yet your expectations remain fundamentally flawed.";
}
