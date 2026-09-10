#include <QApplication>
#include <cstdlib>
#include "avatar_window.h"
#include "memory.h"
#include "llm_engine.h"
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
    qputenv("QT_QPA_PLATFORM", "xcb");
    QApplication app(argc, argv);

    // 1. Initialize SQLite Memory & LLM Engine
    MemoryManager memory("aizen_memory.db");

    // Force save user facts on startup
    memory.saveFact("Name", "Nicho");
    memory.saveFact("OS", "Gentoo Linux with KDE Plasma");
    memory.saveFact("Role", "Creator and user who requires assistance");
    memory.saveFact("Hardware" , "GPU: RTX 3070 ; CPU: Ryzen 7 5700X; RAM: 16GB DDR4");
    memory.saveFact("Interests" , "Military aviation, Japanese sword metallurgy, philosophy, manga/anime, Kenjutsu, firearm mechanics and tactical use, prehistoric biology and software developing");
    memory.saveFact("Birthday" , "January 6th, born in 2009");

    LLMEngine llm("models/aizen_model.gguf");
    llm.loadModel();

    // 2. Launch UI Avatar
    AvatarWindow avatar;
    avatar.show();

    // 3. Multithreaded Terminal Loop
    std::thread inputThread([&avatar, &memory, &llm]() {
        std::string input;
        while (true) {
            std::cout << "\n[You]: ";
            std::getline(std::cin, input);
            
            if (input == "exit") break;

            std::string aiResponse = llm.generateResponse(memory, input);

            memory.addMessage("user", input);
            memory.addMessage("assistant", aiResponse);

            QString qResponse = QString::fromStdString(aiResponse);
            QMetaObject::invokeMethod(&avatar, [&avatar, qResponse]() {
                avatar.updateDialogue(qResponse);
            });
        }
    });

    int execResult = app.exec();
    if (inputThread.joinable()) {
        inputThread.join();
    }
    return execResult;
}
