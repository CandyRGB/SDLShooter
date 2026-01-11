// EntityManager.h
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <list>
#include <memory>
#include <SDL.h>

template<typename T>
class EntityManager {
public:
    void add(std::shared_ptr<T> entity) {
        entities.push_back(entity);
    }

    void clear() {
        entities.clear();
    }

    typename std::list<std::shared_ptr<T>>::iterator erase(typename std::list<std::shared_ptr<T>>::iterator it) {
        return entities.erase(it);
    }

    typename std::list<std::shared_ptr<T>>::iterator begin() {
        return entities.begin();
    }

    typename std::list<std::shared_ptr<T>>::iterator end() {
        return entities.end();
    }

    typename std::list<std::shared_ptr<T>>::const_iterator begin() const {
        return entities.begin();
    }

    typename std::list<std::shared_ptr<T>>::const_iterator end() const {
        return entities.end();
    }

    size_t size() const {
        return entities.size();
    }

private:
    std::list<std::shared_ptr<T>> entities;
};

#endif // ENTITY_MANAGER_H
