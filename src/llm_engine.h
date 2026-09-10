#ifndef LLM_ENGINE_H
#define LLM_ENGINE_H

#include "memory.h"
#include <string>

class LLMEngine {
public:
    explicit LLMEngine(const std::string &modelPath);
    ~LLMEngine();

    bool loadModel();
    std::string generateResponse(MemoryManager &memory, const std::string &userInput);

private:
    std::string modelPath;
    std::string buildPrompt(MemoryManager &memory, const std::string &userInput);
};

#endif // LLM_ENGINE_H
