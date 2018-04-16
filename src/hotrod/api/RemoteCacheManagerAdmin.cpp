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

#define IMPL ((RemoteCacheManagerImpl *) impl)

namespace infinispan {
namespace hotrod {

using namespace operations;


const std::vector<char> RemoteCacheManagerAdmin::CACHE_NAME = {'n', 'a', 'm', 'e'};
const std::vector<char> RemoteCacheManagerAdmin::CACHE_TEMPLATE = {'t','e','m','p','l','a','t','e'};
const std::vector<char> RemoteCacheManagerAdmin::CACHE_CONFIGURATION = {'c','o','n','f','i','g','u','r','a','t','i','o','n'};
const std::vector<char> RemoteCacheManagerAdmin::CACHE_FLAGS = {'f','l','a','g','s'};
const std::vector<char> RemoteCacheManagerAdmin::FLAG_LABELS[] = {{'P','E','R','M','A','N','E','N','T'}};

RemoteCacheManagerAdmin::RemoteCacheManagerAdmin(RemoteCacheManager& cacheManager
                                               , std::function<void(std::string&)> remover
                                               , std::shared_ptr<OperationsFactory> of) : cacheManager(cacheManager) {
    operationsFactory = of;
    this->remover = remover;
}


void RemoteCacheManagerAdmin::executeAdminOperation(std::string adminCmd, std::map<std::vector<char>, std::vector<char> >& param){
    std::vector<char> cmdNameVec(adminCmd.begin(), adminCmd.end());
    std::shared_ptr<AdminOperation> ao(operationsFactory->newAdminOperation(cmdNameVec, param));
    ao->execute();
}

}}
