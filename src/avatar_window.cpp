#include "avatar_window.h"
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QRandomGenerator>
#include <QDirIterator>
#include <QDir>
#include <QDebug>

AvatarWindow::AvatarWindow(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // 1. Setup Audio Engine
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    // 2. Setup Typewriter Timer
    typewriterTimer = new QTimer(this);
    connect(typewriterTimer, &QTimer::timeout, this, &AvatarWindow::revealNextCharacter);

    // 3. Scan Assets Directory
    QString assetDirPath = QCoreApplication::applicationDirPath() + "/../assets/";

    QDirIterator spriteIt(assetDirPath, QStringList() << "*.png" << "*.PNG" << "*.jpg" << "*.JPEG", QDir::Files);
    while (spriteIt.hasNext()) {
        spritePaths.push_back(spriteIt.next());
    }

    QDirIterator audioIt(assetDirPath, QStringList() << "*.wav" << "*.mp3" << "*.ogg", QDir::Files);
    while (audioIt.hasNext()) {
        voicePaths.push_back(audioIt.next());
    }

    // 4. Initial Sprite Setup
    characterLabel = new QLabel(this);
    if (!spritePaths.empty()) {
        QPixmap sprite(spritePaths[0]);
        characterLabel->setPixmap(sprite.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lastSpriteIndex = 0;
    }

    // 5. Speech Bubble Setup
    dialogueBubble = new QLabel("", this);
    dialogueBubble->setFixedWidth(300);
    dialogueBubble->setWordWrap(true);
    dialogueBubble->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    dialogueBubble->setStyleSheet(
        "background-color: rgba(255, 255, 255, 230); "
        "border: 2px solid #4B0082; "
        "border-radius: 12px; "
        "padding: 12px; "
        "font-size: 14px; "
        "color: #111111;"
    );

    // 6. Layout Setup
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(characterLabel, 0, Qt::AlignCenter); 
    layout->addWidget(dialogueBubble, 0, Qt::AlignCenter); 

    setLayout(layout);

    // Set initial dialogue text cleanly
    updateDialogue("What is it that you want from me?");
}

void AvatarWindow::moveToBottomRight() {
    layout()->activate();
    adjustSize();
    
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        int targetX = geo.x() + geo.width() - width() - 20;
        int targetY = geo.y() + geo.height() - height() - 20;
        this->setGeometry(targetX, targetY, width(), height());
    }
}

void AvatarWindow::updateDialogue(const QString &text) {
    // Stop any existing typewriter stream
    typewriterTimer->stop();

    fullTextToDisplay = text;
    currentCharacterIndex = 0;
    dialogueBubble->clear();

    // 1. Pick and set new random sprite frame
    if (!spritePaths.empty()) {
        int spriteIndex = QRandomGenerator::global()->bounded(static_cast<int>(spritePaths.size()));
        
        if (spritePaths.size() > 1 && spriteIndex == lastSpriteIndex) {
            spriteIndex = (spriteIndex + 1) % spritePaths.size();
        }
        
        lastSpriteIndex = spriteIndex;

        QPixmap newSprite(spritePaths[spriteIndex]);
        if (!newSprite.isNull()) {
            characterLabel->clear();
            characterLabel->setPixmap(newSprite.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            characterLabel->update();
        }
    }

    // 2. Play random voice audio
    if (!voicePaths.empty()) {
        int voiceIndex = QRandomGenerator::global()->bounded(static_cast<int>(voicePaths.size()));
        mediaPlayer->setSource(QUrl::fromLocalFile(voicePaths[voiceIndex]));
        mediaPlayer->play();
    }

    // 3. Start typewriter stream: 30ms per character (adjust value for speed)
    typewriterTimer->start(30); 
}

void AvatarWindow::revealNextCharacter() {
    if (currentCharacterIndex < fullTextToDisplay.length()) {
        currentCharacterIndex++;
        dialogueBubble->setText(fullTextToDisplay.left(currentCharacterIndex));
        dialogueBubble->adjustSize();
        moveToBottomRight();
    } else {
        typewriterTimer->stop();
    }
}
