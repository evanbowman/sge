#pragma once

#include <memory>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

#include "Types.hpp"

class CollisionChecker {
public:
    template <typename F>
    void Update(F && onCollision) {
        for (auto it = m_sectors.begin(); it != m_sectors.end();) {
            if (!it->second.dynamicEntities.empty() ||
                !it->second.staticEntities.empty()) {
                UpdateSector(it->first, it->second);
                ++it;
            } else {
                it = m_sectors.erase(it);
            }
        }
    }
    
    void AddDynamicEntity(EntityRef entity);
    void AddStaticEntity(EntityRef entity);

private:
    struct Sector {
        using AList = std::vector<std::pair<WeakEntityRef, WeakEntityRef>>;
        std::vector<WeakEntityRef> staticEntities;
        std::vector<WeakEntityRef> dynamicEntities;
        AList pairs;
        bool pairsAreStale = false;
        AList& GetPairs();
    };
    using Coord = std::pair<int, int>;
    using SectorTree = std::map<Coord, Sector>;

    void DoCollisionTest(Sector& sector);
    void UpdateSector(const Coord& coord, Sector& sector);
    Coord CalcTargetSectorLoc(const Vec2& position);
    Sector& FindOrCreateSector(const Coord& coord);

    SectorTree m_sectors;
    Vec2 m_sectorSize { 128.f, 128.f };
};
