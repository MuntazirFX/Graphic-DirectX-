#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace gdx {

struct ElementDef {
    std::string name;
    std::string file;
    std::string meshFile;
    std::string aniFile;
    std::string type; // PLATTFORM, ENEMY, DECO, BONUS, EXIT, ...
    std::string effect;
    float radius = 0;
    float scaling = 1;
    float speed = 0;
    float jumpHeight = 0;
    float verticalOffset = 0;
    float friction = 0;
    float rotation = 0;
    int walkAnim = -1;
};

class ElementCatalog {
public:
    size_t parse(const std::string& text);
    const ElementDef* find(const std::string& name) const;
    size_t size() const { return defs_.size(); }
    const std::vector<ElementDef>& all() const { return defs_; }
    static ElementCatalog& shared();

private:
    std::vector<ElementDef> defs_;
    std::unordered_map<std::string, size_t> byName_;
    std::unordered_map<std::string, size_t> byLower_;
};

} // namespace gdx
