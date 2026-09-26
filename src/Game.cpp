#include "gdx/Game.h"
#include <algorithm>
#include <cctype>

namespace gdx {
namespace {

std::string up(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

} // namespace

ActorKind kindFromType(const std::string& type) {
    const std::string t = up(type);
    if (t == "PLATTFORM" || t == "PLATFORM") return ActorKind::Platform;
    if (t == "RECTFORM") return ActorKind::RectForm;
    if (t == "DECO") return ActorKind::Deco;
    if (t == "ENEMY" || t == "ELEVATORENEMY") return ActorKind::Enemy;
    if (t == "ELEVATOR") return ActorKind::Elevator;
    if (t == "MOVER") return ActorKind::Mover;
    if (t == "JUMPER") return ActorKind::Jumper;
    if (t == "BONUS") return ActorKind::Bonus;
    if (t == "EXTRALIFE") return ActorKind::ExtraLife;
    if (t == "SAVEPOINT") return ActorKind::SavePoint;
    if (t == "EXIT") return ActorKind::Exit;
    return ActorKind::Unknown;
}

void buildSession(const DatLevel& level, const ElementCatalog& cat, GameSession& s, GameConfig cfg) {
    s = {};
    s.cfg = cfg;
    s.lives = cfg.startLives;
    s.timeLeft = cfg.defaultTimeSec;
    s.actors.reserve(level.entities.size());
    int firstPlat = -1;
    for (const auto& e : level.entities) {
        Actor a;
        a.ent = e;
        a.def = cat.find(e.name);
        a.kind = a.def ? kindFromType(a.def->type) : ActorKind::Unknown;
        if (a.kind == ActorKind::Unknown) {
            const std::string n = up(e.name);
            if (n.find("PRESENT") != std::string::npos) a.kind = ActorKind::Bonus;
            else if (n.find("EXIT") != std::string::npos) a.kind = ActorKind::Exit;
            else if (n.find("EXTRA") != std::string::npos) a.kind = ActorKind::ExtraLife;
            else if (n.find("SAVE") != std::string::npos) a.kind = ActorKind::SavePoint;
            else if (n.find("TROLL") != std::string::npos || n.find("RABE") != std::string::npos)
                a.kind = ActorKind::Enemy;
        }
        if (a.kind == ActorKind::Bonus) ++s.totalPresents;
        if (firstPlat < 0 && (a.kind == ActorKind::Platform || a.kind == ActorKind::RectForm))
            firstPlat = static_cast<int>(s.actors.size());
        s.actors.push_back(a);
    }
    s.spawnIndex = firstPlat;
}

void tickTimer(GameSession& s, float dt) {
    if (s.exited) return;
    s.timeLeft -= dt;
    if (s.timeLeft <= 0.f) {
        s.timeLeft = 0.f;
        s.timeUp = true; // PC: timer 0 does not auto-fail the level
    }
}

bool tryCollect(GameSession& s, size_t i) {
    if (i >= s.actors.size()) return false;
    Actor& a = s.actors[i];
    if (a.collected) return false;
    if (a.kind == ActorKind::Bonus) {
        a.collected = true;
        ++s.collectedPresents;
        s.score += static_cast<float>(s.cfg.presentPoints);
        if (s.collectedPresents == s.totalPresents && s.totalPresents > 0)
            s.lives += s.cfg.extraLifeAtAllPresents;
        return true;
    }
    if (a.kind == ActorKind::ExtraLife) {
        a.collected = true;
        ++s.lives;
        return true;
    }
    return false;
}

bool tryExit(GameSession& s) {
    if (s.exited) return false;
    s.exited = true;
    s.score += static_cast<float>(levelCompletePoints(s) - static_cast<int>(s.score));
    // levelCompletePoints includes present points + time; rebuild cleanly:
    s.score = static_cast<float>(s.collectedPresents * s.cfg.presentPoints +
                                 static_cast<int>(s.timeLeft) * s.cfg.secondPoints);
    s.totalScore += s.score;
    return true;
}

int levelCompletePoints(const GameSession& s) {
    return s.collectedPresents * s.cfg.presentPoints +
           static_cast<int>(s.timeLeft) * s.cfg.secondPoints;
}

} // namespace gdx
