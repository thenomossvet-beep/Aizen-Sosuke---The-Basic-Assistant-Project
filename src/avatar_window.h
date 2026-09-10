#ifndef AVATAR_WINDOW_H
#define AVATAR_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>
#include <vector>

class AvatarWindow : public QWidget {
    Q_OBJECT

public:
    explicit AvatarWindow(QWidget *parent = nullptr);
    void updateDialogue(const QString &text);
    void moveToBottomRight();

private slots:
    void revealNextCharacter(); // Slot triggered by timer to stream text

private:
    QLabel *characterLabel;
    QLabel *dialogueBubble;

    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
    
    std::vector<QString> spritePaths;
    std::vector<QString> voicePaths;
    
    int lastSpriteIndex = -1;

    // Typewriter state tracking
    QTimer *typewriterTimer;
    QString fullTextToDisplay;
    int currentCharacterIndex = 0;
};

#endif // AVATAR_WINDOW_H
