#include <cmath> 

#include "CollisionChecker.hpp"
#include "Entity.hpp"

CollisionChecker::Coord CollisionChecker::CalcTargetSectorLoc(const Vec2& position) {
    return {
        std::floor(position.x / m_sectorSize.x),
        std::floor(position.y / m_sectorSize.y)
    };
}

CollisionChecker::Sector& CollisionChecker::FindOrCreateSector(const Coord& coord) {
    auto foundSector = m_sectors.find(coord);
    if (foundSector == m_sectors.end()) {
        auto createdSector = m_sectors.insert({coord, {}});
        return createdSector.first->second;
    }
    return foundSector->second;
}

void CollisionChecker::AddDynamicEntity(EntityRef entity) {
    const auto targetSectorLoc = CalcTargetSectorLoc(entity->GetPosition());
    auto& sector = FindOrCreateSector(targetSectorLoc);
    sector.dynamicEntities.push_back(entity);
    sector.pairsAreStale = true;
}

void CollisionChecker::AddStaticEntity(EntityRef entity) {
    const auto targetSectorLoc = CalcTargetSectorLoc(entity->GetPosition());
    auto& sector = FindOrCreateSector(targetSectorLoc);
    sector.staticEntities.push_back(entity);
    sector.pairsAreStale = true;
}

CollisionChecker::Sector::AList& CollisionChecker::Sector::GetPairs() {
    if (this->pairsAreStale) {
        this->pairs.clear();
        for (size_t i = 0; i < this->dynamicEntities.size(); ++i) {
            for (size_t j = i + 1; j < this->dynamicEntities.size(); ++j) {
                this->pairs.push_back({this->dynamicEntities[i],
                                       this->dynamicEntities[j]});
            }
            for (size_t j = 0; j < this->staticEntities.size(); ++j) {
                this->pairs.push_back({this->dynamicEntities[i],
                                       this->staticEntities[j]});
            }
        }
        this->pairsAreStale = false;
    }
    return this->pairs;
}

void CollisionChecker::DoCollisionTest(Sector& sector) {
    auto& pairs = sector.GetPairs();
    for (auto& pair : pairs) {
        auto lhs = pair.first.lock();
        auto rhs = pair.second.lock();
        if (!lhs || !rhs) {
            continue;
        }
        // TODO:
        // if (lhs->GetCollisionComponent().GetBounds().Intersects(etc...))
    }
}

void CollisionChecker::UpdateSector(const Coord& coord, Sector& sector) {
    for (auto it = sector.dynamicEntities.begin();
         it != sector.dynamicEntities.end();) {
        if (auto entity = it->lock()) {
            auto currentLoc = CalcTargetSectorLoc(entity->GetPosition());
            if (currentLoc != coord) {
                it = sector.dynamicEntities.erase(it);
                m_sectors[currentLoc].dynamicEntities.push_back(entity);
                m_sectors[currentLoc].pairsAreStale = true;
                sector.pairsAreStale = true;
            } else {
                ++it;
            }
        } else {
            it = sector.dynamicEntities.erase(it);
            sector.pairsAreStale = true;
        }
    }
    for (auto it = sector.staticEntities.begin();
         it != sector.staticEntities.end();) {
        if (!it->lock()) {
            it = sector.staticEntities.erase(it);
            sector.pairsAreStale = true;
        } else {
            ++it;
        }
    }
    DoCollisionTest(sector);
}
