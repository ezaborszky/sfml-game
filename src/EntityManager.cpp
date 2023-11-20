#include "EntityManager.h"
#include <iostream>
#include <algorithm>

EntityManager::EntityManager(){};

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    // give each entity an icreasing integer id
    auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
    m_entitiesToAdd.push_back(e);
    return e;
}

void EntityManager::removeDeadEntities(EntityVec &vec)
{
    auto del = std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity> &entity)
                              { return !entity->isActive(); });
    vec.erase(del, vec.end());
}

void EntityManager::update()
{
    for (auto e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }

    removeDeadEntities(m_entities);

    for (auto &[tag, entities] : m_entityMap)
        removeDeadEntities(entities);

    m_entitiesToAdd.clear();

    // update
}

const EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec &EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
};