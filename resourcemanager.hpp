#pragma once

#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

template<class Resource, class Identifier>
class ResourceManager
{
public:
    void load(Identifier id, const std::string &fileName);
    template<class Parameter>
    void load(Identifier id, const std::string &fileName, const Parameter &parameter);
    Resource& get(Identifier id);
    const Resource& get(Identifier id) const;
private:
    std::map<Identifier, std::unique_ptr<Resource>> mResourceMap;
    void insertResource(Identifier id, std::unique_ptr<Resource> &resource);
};

#include "resourcemanager.impl"

