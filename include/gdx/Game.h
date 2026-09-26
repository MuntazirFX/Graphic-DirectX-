#pragma once

#include "gdx/DatLevel.h"
#include "gdx/ElementCatalog.h"
#include <cstdint>
#include <string>
#include <vector>

namespace gdx {

enum class ActorKind {
    Unknown,
    Platform,
    Deco,
    Enemy,
    Elevator,
    Mover,
    Jumper,
    Bonus,
    ExtraLife,
    SavePoint,
    Exit,
    RectForm
};

struct Actor {
    DatEntity ent;
    ActorKind kind = ActorKind::Unknown;
    const ElementDef* def = nullptr;
    bool collected = false;
};

struct GameConfig {
    int startLives = 3;
    int presentPoints = 10;
    int secondPoints = 2;
    int extraLifeAtAllPresents = 1; // +lives when last present taken
    float defaultTimeSec = 7 * 60.f; // level 1 ~ 7:00 on PC
};

struct GameSession {
    GameConfig cfg;
    std::vector<Actor> actors;
    int totalPresents = 0;
    int collectedPresents = 0;
    int lives = 3;
    float timeLeft = 0;
    float score = 0;
    float totalScore = 0;
    bool exited = false;
    bool timeUp = false;
    int spawnIndex = -1;

    const Actor* spawn() const {
        if (spawnIndex < 0 || spawnIndex >= static_cast<int>(actors.size())) return nullptr;
        return &actors[static_cast<size_t>(spawnIndex)];
    }
};

ActorKind kindFromType(const std::string& type);
void buildSession(const DatLevel& level, const ElementCatalog& cat, GameSession& s,
                  GameConfig cfg = {});
void tickTimer(GameSession& s, float dt);
bool tryCollect(GameSession& s, size_t actorIndex);
bool tryExit(GameSession& s);
int levelCompletePoints(const GameSession& s);

} // namespace gdx
