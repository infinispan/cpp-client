/*
 * RemoteCacheManagerAdmin.cpp
 *
 *  Created on: Apr 10, 2018
 *      Author: rigazilla
 */

#include "infinispan/hotrod/RemoteCacheManagerAdmin.h"
#include "hotrod/impl/Topology.h"
#include "hotrod/impl/operations/OperationsFactory.h"
#include "hotrod/impl/operations/AdminOperation.h"
#include "algorithm"

#define IMPL ((RemoteCacheManagerImpl *) impl)

namespace infinispan {
namespace hotrod {

using namespace operations;

const std::string RemoteCacheManagerAdmin::FLAG_LABELS[] = {"PERMANENT"};

RemoteCacheManagerAdmin::RemoteCacheManagerAdmin(RemoteCacheManager& cacheManager
                                               , std::function<void(std::string&)> remover
                                               , std::shared_ptr<OperationsFactory> of) : cacheManager(cacheManager) {
    operationsFactory = of;
    this->remover = remover;
}


std::vector<char> RemoteCacheManagerAdmin::executeAdminOperation(std::string adminCmd, std::map<std::string, std::string>& param){
    std::vector<char> cmdNameVec(adminCmd.begin(), adminCmd.end());
    std::map<std::vector<char>, std::vector<char> > vec_param;
    for (auto it = param.begin(); it != param.end(); it++)
    {
        vec_param[std::vector<char>(it->first.begin(), it->first.end())] = std::vector<char>(it->second.begin(), it->second.end());
    }
    std::shared_ptr<AdminOperation> ao(operationsFactory->newAdminOperation(cmdNameVec, vec_param));
    return ao->execute();
}

void RemoteCacheManagerAdmin::createCache(const std::string name, std::string model, std::string command) {
    std::map<std::string, std::string > params;
     params["name"] = name;
     if (!model.empty())
     {
         params["template"] = model;
     }
     if (!flags.empty()) {
         params["flags"] = flags2Params(flags);
     }
     executeAdminOperation(command , params);
}

void RemoteCacheManagerAdmin::createCacheWithXml(const std::string name, std::string conf, std::string command) {
    std::map<std::string, std::string > params;
     params["name"] = name;
     if (!conf.empty())
     {
         params["configuration"] = conf;
     }
     if (!flags.empty()) {
         params["flags"] = flags2Params(flags);
     }
     executeAdminOperation(command , params);
}

void RemoteCacheManagerAdmin::removeCache(std::string name) {
    // remove cache from map
    this->remover(name);
    std::map<std::string, std::string > params;
    params["name"] = name;
    if (!flags.empty()) {
        params["flags"] = flags2Params(flags);
    }
    executeAdminOperation("@@cache@remove" , params);
}

void RemoteCacheManagerAdmin::reindexCache(std::string name) {
    // remove cache from map
    std::map<std::string, std::string > params;
    params["name"] = name;
    executeAdminOperation("@@cache@reindex" , params);
}

std::set<std::string> RemoteCacheManagerAdmin::getCacheNames() {
    std::map<std::string, std::string > params;
    std::vector<char> resultAsJson = executeAdminOperation("@@cache@names" , params);
    // Hardcoded digesting of json array of strings.
    // We're supposing cache names doesn't contain " (double quote)
    std::set<std::string> result;
    auto found_begin = resultAsJson.begin(), found_end = resultAsJson.begin();
    // from the first "
    while ( (found_begin = std::find(found_end, resultAsJson.end(), '"')) != resultAsJson.end() ) {
        // find the next " and build a string with everything in between
        found_end = std::find(++found_begin, resultAsJson.end(), '"');
        std::string s(found_begin, found_end);
        // ignore empty string
        if (!s.empty()) {
            result.insert(s);
        }
        found_end++;
    }
    return result;
}

RemoteCacheManagerAdmin& RemoteCacheManagerAdmin::withFlags(std::set<AdminFlag> flags)
{
    this->flags=flags;
    return *this;
}

std::string RemoteCacheManagerAdmin::flags2Params(const std::set<AdminFlag>& flags) const {
    std::string ret;
    for(auto it = flags.begin(); it!=flags.end(); it++)
    {
        if (it!=flags.begin())
        {
            ret.push_back(',');
        }
        ret += FLAG_LABELS[*it];
    }
    return ret;
}


}}
