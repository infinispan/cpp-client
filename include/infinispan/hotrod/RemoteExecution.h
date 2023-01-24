/*
 * RemoteExecution.h
 *
 *  Created on: Dec 5, 2017
 *      Author: rigazilla
 */

#ifndef INCLUDE_INFINISPAN_HOTROD_REMOTEEXECUTION_H_
#define INCLUDE_INFINISPAN_HOTROD_REMOTEEXECUTION_H_

#include "infinispan/hotrod/ImportExport.h"
#include "infinispan/hotrod/JBossMarshaller.h"
#include "infinispan/hotrod/RemoteCacheBase.h"

namespace infinispan {
namespace hotrod {

/**
 * Simplify and improve the call of a remote execution task:
 *  - no more direct (un)marshalling is needed
 *  - args value can be of differents type
 *  - return value is statically typed
 *  - no need to explicitly open the __script_cache cache
 *
 *  A Marshaller must be provided as template arguments. JBossMarshaller
 *  can be used as reference.
 */
template<class M = JBossMarshaller, class K>
class RemoteExecution {
public:
    RemoteExecution(RemoteCacheBase& cache) :
            cache(cache) {
    }

    /**
     * Execute a remote task
     *
     * \param s name of the script to invlke
     * \return the result of the execution
     * \tparam ResultType the type of the result value
     */
    template<typename ResultType>
    ResultType execute(const std::string& s) {
        std::vector<char> buff(s.begin(), s.end());
        return M::template unmarshall<ResultType>(cache.base_execute(buff, m));
    }

    /**
     * Execute a remote task
     * \param k a key used to target a specific node in the topology
     * \param s name of the script to invoke
     * \return the result of the execution
     * \tparam ResultType the type of the result value
     */
    template<typename ResultType>
    ResultType execute(const std::string& s, K& key) {
        std::vector<char> buff(s.begin(), s.end());
        return M::template unmarshall<ResultType>(cache.base_execute(&key, buff, m));
    }

    /**
     * Add a name,value pair to the arguments list
     *
     * \param s name of the arg
     * \param v value of the arg
     * \tparam ArgType is the type of the arg value. It can be of any type
     * as long as the Marshaller is equipped with the related implementation
     */
    template<typename ArgType>
    void addArg(std::string s, ArgType v) {
        std::vector<char> buffK(s.begin(), s.end());
        std::vector<char> buffV(M::marshall(v));
        m[buffK] = buffV;
    }

    /**
     * Install a script on the server
     *
     * \param name the name of the script
     * \param script the code
     */
    void putScript(const std::string& name, const std::string& script) {
        std::vector<char> buffName(M::marshall(name));
        std::vector<char> buffScript(M::marshall(script));
        cache.putScript(buffName, buffScript);
    }

private:
    std::map<std::vector<char>, std::vector<char>> m;
    RemoteCacheBase& cache;
};

}
}

#endif /* INCLUDE_INFINISPAN_HOTROD_REMOTEEXECUTION_H_ */
