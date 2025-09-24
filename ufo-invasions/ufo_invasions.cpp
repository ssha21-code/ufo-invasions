#include "raylib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include "raymath.h"

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;

Texture2D textureBackground;
Texture2D textureBeam;
Texture2D textureUfo;
Texture2D texturePlayer;
Texture2D textureAK47;
Texture2D textureBullet;
Texture2D textureStart;
Texture2D texturePause;
Texture2D texturePlay;

Sound soundSelection;
Sound soundGunshot;
Sound soundHit;
Music bossMusic;

void loadMedia() {
    Image imageBackground = LoadImage("images/background.png");
    Image imageBeam = LoadImage("images/beam.png");
    Image imageUfo = LoadImage("images/ufo.png");
    Image imagePlayer = LoadImage("images/player.png");
    Image imageAK47 = LoadImage("images/ak47.png");
    Image imageBullet = LoadImage("images/bullet.png");
    Image imageStart = LoadImage("images/start_button.png");
    Image imagePause = LoadImage("images/pause_button.png");
    Image imagePlay = LoadImage("images/play_button.png");

    soundSelection = LoadSound("audio/selection.wav");
    soundGunshot = LoadSound("audio/gunshot.wav");
    soundHit = LoadSound("audio/hit.wav");
    bossMusic = LoadMusicStream("audio/boss_music.wav");
    bossMusic.looping = true;

    textureBackground = LoadTextureFromImage(imageBackground);
    textureBeam = LoadTextureFromImage(imageBeam);
    textureUfo = LoadTextureFromImage(imageUfo);
    texturePlayer = LoadTextureFromImage(imagePlayer);
    textureAK47 = LoadTextureFromImage(imageAK47);
    textureBullet = LoadTextureFromImage(imageBullet);
    textureStart = LoadTextureFromImage(imageStart);
    texturePause = LoadTextureFromImage(imagePause);
    texturePlay = LoadTextureFromImage(imagePlay);

    UnloadImage(imageBackground);
    UnloadImage(imageBeam);
    UnloadImage(imageUfo);
    UnloadImage(imagePlayer);
    UnloadImage(imageAK47);
    UnloadImage(imageBullet);
    UnloadImage(imageStart);
    UnloadImage(imagePause);
    UnloadImage(imagePlay);
}

class Player {
public: 
    float width = texturePlayer.width;
    float height = texturePlayer.height;
    float posX;
    float posY;
    float groundY = 600.0f;
    Vector2 position = {};
    Vector2 velocity = {8.0f, 8.0f};
    Vector2 center = {};
    float gravity = 0.5f;
    float jumpStrength = -12.0f;
    float horizontalDirection = width;
    int hp = 50;
    Rectangle hitbox = {};

    bool isOnGround = true;
    bool shouldShoot = false;
    bool isDead = false;

    Player() {
        this->width  = texturePlayer.width;
        this->height = texturePlayer.height;
        this->posX = (WINDOW_WIDTH - width) / 2.0f;
        this->posY = groundY - height;
        this->position = {posX, posY};
        this->hitbox = {position.x, position.y, width, height};
        this->center = {position.x + width / 2.0f, position.y + height / 2.0f};
    }

    void draw() {
        Rectangle src = {0.0f, 0.0f, horizontalDirection, height};
        Rectangle dest = hitbox;
        Vector2 origin = {0.0f, 0.0f};
        DrawTexturePro(texturePlayer, src, dest, origin, 0.0f, WHITE);
    }

    void update() {
        if (IsKeyDown(KEY_A)) {
            if (position.x > 0) {
                position.x -= velocity.x;
            }
        }
        if (IsKeyDown(KEY_D)) {
            if (position.x + width < WINDOW_WIDTH) {
                position.x += velocity.x;
            }
        }
        velocity.y += gravity;
        position.y += velocity.y;
        if (position.y + height >= groundY) {
            position.y = groundY - height;
            velocity.y = 0;
            isOnGround = true;
        } else {
            isOnGround = false;
        }
        if (IsKeyPressed(KEY_SPACE) && isOnGround) {
            velocity.y = jumpStrength;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            PlaySound(soundGunshot);
            shouldShoot = true;
        }
        center = {position.x + width / 2.0f, position.y + height / 2.0f};
        hitbox = {position.x, position.y, width, height};
    }

};

class UFO {
public:
    float width;
    float height;
    Vector2 position = {};
    Vector2 velocity = {6.0f, 6.0f};
    Vector2 center = {};
    Rectangle hitbox = {};
    Vector2 beamPosition = {};
    Rectangle beamHitbox = {};
    Player *player;
    double lastUpdateTime = 0;
    int hp = 15;
    int wavesBeaten;
    int maximumBeamDamage = 3;

    bool hasReachedPosition = false;
    bool canChaseAfterPlayer = false;
    bool shouldBeDestroyed = false;

    UFO(float posX, Player *player, int *wavesBeaten) {
        this->position = {posX, -1000.0f};
        this->player = player;
        this->width = textureUfo.width;
        this->height = textureUfo.height;
        this->center = {position.x + width / 2.0f, position.y + height / 2.0f};
        this->beamHitbox = {center.x - beamHitbox.width / 2.0f, position.y + height, (float)textureBeam.width, (float)textureBeam.height};
        this->beamPosition = {center.x - beamHitbox.width / 2.0f, position.y + height};
        this->wavesBeaten = *wavesBeaten;
    }

    void draw() {
        Rectangle src = {0.0f, 0.0f, width, height};
        Rectangle dest = {position.x, position.y, width, height};
        Vector2 origin = {0, 0};
        DrawTexturePro(textureUfo, src, dest, origin, 0.0f, WHITE);
        if (canChaseAfterPlayer && hasReachedPosition) {
            DrawTexture(textureBeam, beamPosition.x, beamPosition.y, Color{255, 255, 255, 150});
        }
        DrawText(TextFormat("%i", hp), position.x + width / 2.0f, position.y - 35, 25, WHITE);
    }

    void update() {
        if (hasReachedPosition) {
            if (canChaseAfterPlayer) {
                if (fabs(center.x - player->position.x) > 20.0f) {
                    if (player->position.x > center.x) {
                        position.x += velocity.x;
                    }
                    if (player->position.x < center.x) {
                        position.x -= velocity.x;
                    }
                }
                beamPosition = {center.x - beamHitbox.width / 2.0f, position.y + height};
                beamHitbox = {beamPosition.x, beamPosition.y, (float)textureBeam.width, (float)textureBeam.height};
            }
        } else {
            position.y += velocity.y;
            if (position.y >= 150.0f) {
                position.y = 150.0f;
                hasReachedPosition = true;
            }
        }
        if (wavesBeaten < 5) {
            maximumBeamDamage = 3;
        } else if (wavesBeaten < 10) {
            maximumBeamDamage = 4;
        } else {
            maximumBeamDamage = 5;
        }
        if (CheckCollisionRecs(player->hitbox, beamHitbox)) {
            double currentTime = GetTime();
            if (currentTime - lastUpdateTime >= 0.5) {
                player->hp -= GetRandomValue(1, maximumBeamDamage);
                lastUpdateTime = currentTime;
                PlaySound(soundHit);
                if (player->hp <= 0) {
                    player->hp = 0;
                    player->isDead = true;
                }
            }
        }
        center = {position.x + width / 2.0f, position.y + height / 2.0f};
        hitbox = {position.x, position.y, width, height};
    }
};

class AK47 {
public: 
    float width;
    float height;
    float angleRad;
    float angleDeg;
    float horizontalDirection;
    float verticalDirection;
    Vector2 position = {};
    Vector2 center = {};
    Rectangle hitbox = {};
    Player *player;

    AK47(Player *player) {
        this->player = player;
        this->width = textureAK47.width;
        this->height = textureAK47.height;
        this->position = {player->center.x - width / 2.0f, player->center.y - height / 2.0f};
        this->center = {position.x + width / 2.0f, position.y + height / 2.0f};
        this->hitbox = {position.x, position.y, width, height};
        this->angleDeg = 0.0f;
        this->angleRad = 0.0f;
        this->horizontalDirection = width;
        this->verticalDirection = height;
    }

    void draw() {
        float drawWidth  = width;
        float drawHeight = (angleDeg > 90 || angleDeg < -90) ? -height : height;
        Vector2 origin = {width / 2.0f, height / 2.0f};
        Rectangle src = {0, 0, drawWidth, drawHeight};
        Rectangle dest = {position.x + width / 2, position.y + height / 2, width, height};
        DrawTexturePro(textureAK47, src, dest, origin, angleDeg, WHITE);
    }

    void update() {
        position.x = player->center.x - width / 2.0f;
        position.y = player->center.y - height / 2.0f;
        center = {position.x + width / 2.0f, position.y + height / 2.0f};
        Vector2 mousePos = GetMousePosition();
        float dx = mousePos.x - center.x;
        float dy = mousePos.y - center.y;
        angleRad = atan2(dy, dx);
        angleDeg = angleRad * RAD2DEG;
        if (angleDeg > 90 || angleDeg < -90) {
            player->horizontalDirection = -player->width;
        } else {
            player->horizontalDirection = player->width;
        }
    }
};

class Bullet {
public: 
    float width;
    float height;
    float posX;
    float posY;
    float velocityX = 20.0f;
    float velocityY = 20.0f;
    float angleRad;
    float angleDeg;
    Vector2 position = {};
    Vector2 origin = {};
    Vector2 velocity = {};
    Rectangle src = {0, 0, width, height};
    Rectangle hitbox = {posX, posY, width, height};

    bool shouldBeDestroyed = false;

    Bullet(Vector2 position, float angleRad) {
        this->width  = textureBullet.width;
        this->height = textureBullet.height;
        this->position = position;
        this->angleRad = angleRad;
        this->angleDeg = angleRad * (180.0f / PI);
        this->origin = {width / 2.0f, height / 2.0f};
        this->src = {0, 0, width, height};
        this->hitbox = {position.x - width / 2.0f, position.y - height / 2.0f, width, height};
    }

    void draw() {
        Rectangle dest = {position.x, position.y, width, height};
        DrawTexturePro(textureBullet, src, dest, origin, angleDeg, WHITE);
    }

    void update() {
        velocityX = cos(angleRad) * 20.0f;
        velocityY = sin(angleRad) * 20.0f;
        velocity = {velocityX, velocityY};
        position = Vector2Add(position, velocity);

        hitbox = {position.x - width / 2.0f, position.y - height / 2.0f, width, height};
    }
};

class Game {
public: 
    Player player;
    AK47 *ak47 = nullptr;
    std::vector<UFO> ufos = {};
    std::vector<Bullet> bullets = {};
    std::vector<int> scores = {};
    Color translucentWhite = Color{255, 255, 255, 125};

    int ufoSpawnNumber = 3;
    int wavesBeaten = -1;
    int highScore = 0;

    bool shouldSpawnUfo = false;
    bool isInHomeScreen = true;
    bool isPaused = true;

    float startButtonWidth = 200.0f;
    float startButtonHeight = 90.0f;
    Vector2 startButtonPos = {(WINDOW_WIDTH - startButtonWidth) / 2.0f, 500.0f};
    Rectangle startButtonHitbox = {startButtonPos.x, startButtonPos.y, startButtonWidth, startButtonHeight};

    float pausePlayButtonWidth = 100.0f;
    float pausePlayButtonHeight = 100.0f;
    Vector2 pausePlayButtonPos = {WINDOW_WIDTH - 150.0f, 50.0f};
    Rectangle pausePlayButtonHitbox = {pausePlayButtonPos.x, pausePlayButtonPos.y, pausePlayButtonWidth, pausePlayButtonHeight};

    Game() {
        this->player = Player();
        this->ak47 = new AK47(&player);
        PlayMusicStream(bossMusic);
    }

    void draw() {
        DrawText("Please Download Textures and Sounds", 100, 400, 40, WHITE);
        DrawTexture(textureBackground, 0, 0, WHITE);
        if (!isInHomeScreen) {
            player.draw();
            ak47->draw();
            for (auto &ufo: ufos) {
                ufo.draw();
            }
            for (auto &bullet: bullets) {
                bullet.draw();
            }
            DrawText(TextFormat("Health: %i", player.hp), 45, 45, 36, translucentWhite);
            DrawText(TextFormat("Waves Beaten: %i", wavesBeaten), 45, 90, 36, translucentWhite);
            if (isPaused) {
                DrawTexture(texturePlay, pausePlayButtonPos.x, pausePlayButtonPos.y, WHITE);
            } else {
                DrawTexture(texturePause, pausePlayButtonPos.x, pausePlayButtonPos.y, WHITE);
            }
        } else {
            float textPosX = (WINDOW_WIDTH - MeasureText("UFO INVASIONS", 100)) / 2.0f;
            DrawText("UFO INVASIONS", textPosX, 200, 100, translucentWhite);
            DrawTexture(textureStart, startButtonPos.x, startButtonPos.y, WHITE);
            if (!scores.empty()) {
                DrawText(TextFormat("High Score: %i", highScore), 50, 400, 34, translucentWhite);
            }
        }
    }

    void update() {
        if (!isPaused) {
            player.update();
            ak47->update();
            if (ufos.empty()) {
                wavesBeaten++;
                shouldSpawnUfo = true;
            }
            if (shouldSpawnUfo) {
                player.hp = 50;
                float spawnX = 150.0f;
                float distance = 270.0f;
                for (int i = 0; i < ufoSpawnNumber; i++) {
                    ufos.push_back(UFO(spawnX + distance * i, &player, &wavesBeaten));
                }
                shouldSpawnUfo = false;
            }
            ufos.at(0).canChaseAfterPlayer = true;
            if (player.shouldShoot) {
                float forwardOffset = ak47->width * 0.55f;    
                float verticalOffset = ak47->height * -0.20f;  
    
                Vector2 forward = { cos(ak47->angleRad), sin(ak47->angleRad) };
                Vector2 perp    = { -sin(ak47->angleRad), cos(ak47->angleRad) };
                if (ak47->angleDeg > 90 || ak47->angleDeg < -90) {
                    perp = Vector2Negate(perp);
                }
    
                Vector2 spawnPos = Vector2Add(
                    ak47->center,
                    Vector2Add(Vector2Scale(forward, forwardOffset), Vector2Scale(perp, verticalOffset))
                );
    
                bullets.push_back(Bullet(spawnPos, ak47->angleRad));
                player.shouldShoot = false;
            }
            if (player.isDead) {
                scores.push_back(wavesBeaten);
                calculateHighScore();
                ufos.clear();
                bullets.clear();
                wavesBeaten = -1;
                isInHomeScreen = true;
                isPaused = true;
                player.isDead = false;
            }
            for (auto &ufo: ufos) {
                ufo.update();
            }
            for (auto &bullet: bullets) {
                bullet.update();
            }
            UpdateMusicStream(bossMusic);
            checkRemoval();
            garbageCollect();
            checkCollision();
        } else if (isPaused && isInHomeScreen) {
            Vector2 mousePos = GetMousePosition();
            if (CheckCollisionPointRec(mousePos, startButtonHitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                PlaySound(soundSelection);
                isPaused = false;
                isInHomeScreen = false;
            }
        }
        Vector2 mousePos = GetMousePosition();
        if (CheckCollisionPointRec(mousePos, pausePlayButtonHitbox) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isInHomeScreen) {
            PlaySound(soundSelection);
            isPaused = !isPaused;
        }
        if (!IsWindowFocused() || IsWindowMinimized()) {
            isPaused = true;
        }
    }
    void checkRemoval() {
        for (auto &bullet: bullets) {
            if (bullet.position.x + bullet.width - 20.0f > WINDOW_WIDTH || bullet.position.x < 0 || bullet.position.y + bullet.height > player.groundY || bullet.position.y < 0) {
                bullet.shouldBeDestroyed = true;
            }
        }
    }
    void garbageCollect() {
        for (int i = 0; i < bullets.size(); i++) {
            if (bullets.at(i).shouldBeDestroyed) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < ufos.size(); i++) {
            if (ufos.at(i).shouldBeDestroyed) {
                ufos.erase(ufos.begin() + i);
                i--;
            }
        }
    }
    void checkCollision() {
        for (auto &ufo: ufos) {
            for (auto &bullet: bullets) {
                if (CheckCollisionRecs(ufo.hitbox, bullet.hitbox)) {
                    if (ufo.hp > 0) {
                        ufo.hp--;
                        if (ufo.hp <= 0) {
                            ufo.shouldBeDestroyed = true;
                        }
                    }
                    PlaySound(soundHit);
                    bullet.shouldBeDestroyed = true;
                }
            }
        }
    }
    void calculateHighScore() {
        if (scores.empty()) return;
        int highestScore = 0;
        for (int score: scores) {
            if (highestScore > score) {
                continue;
            } else if (score >= highestScore) {
                highestScore = score;
            }
        }
        highScore = highestScore;
    }
};



int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "UFO Invasions");
    InitAudioDevice();
    SetTargetFPS(60);

    loadMedia();
    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        game.update();
        game.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}