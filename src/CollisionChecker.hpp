#pragma once

#include <memory>
#include <map>
#include <utility>
#include <vector>

#include "Types.hpp"

class CollisionChecker {
public:
    void Update();
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
    std::mutex m_sectorsMtx;
    Vec2 m_sectorSize { 128.f, 128.f };
};
