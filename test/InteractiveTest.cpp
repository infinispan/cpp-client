#include <cstdio>
#include <cstdlib>
#include <vector>
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"
#include "hotrod/impl/RemoteCacheManagerImpl.h"
#include "hotrod/impl/transport/tcp/TcpTransportFactory.h"

using namespace infinispan::hotrod;
using namespace infinispan::hotrod::transport;

typedef RemoteCache<std::string, std::string> StringCache;
typedef unsigned char *ptr_t;
typedef std::vector<InetSocketAddress> AddrVector;

static AddrVector getServers(RemoteCacheManager &manager) {
	// kids, don't try this at home
	// works only for certain version compiled for 64bit
	HR_SHARED_PTR<RemoteCacheManagerImpl> pImpl = *reinterpret_cast<HR_SHARED_PTR<RemoteCacheManagerImpl> *>(&manager);
	ptr_t impl = reinterpret_cast<ptr_t>(&*pImpl);
	HR_SHARED_PTR<TransportFactory> pFactory = *reinterpret_cast<HR_SHARED_PTR<TransportFactory> *>(impl + 0xe0);
	ptr_t factory = reinterpret_cast<ptr_t>(&*pFactory);
	return *reinterpret_cast<AddrVector *>(factory + 0x30);
}

static void printServers(RemoteCacheManager &manager) {
	std::vector<InetSocketAddress> servers = getServers(manager);
	std::cout << "Servers:\n";
	for (AddrVector::iterator it = servers.begin(); it != servers.end(); ++it) {
		std::cout << it->getHostname() << ":" << it->getPort() << std::endl;
	}
}

static void doGet(StringCache &cache) {
	std::cout << "Key: ";
	std::string key;
	std::cin >> key;
	std::string *pvalue = cache.get(key);
	std::string value = pvalue == NULL ? std::string("-null-") : *pvalue;
	std::cout << "Value: " << value << "\n";
	delete pvalue;
}

static void doPut(StringCache &cache) {
	std::cout << "Key: ";
	std::string key, value;
	std::cin >> key;
	std::cout << "Value: ";
	std::cin >> value;
	std::string *pprev = cache.put(key, value);
	std::string prev = pprev == NULL ? std::string("-null-") : *pprev;
	std::cout << "Previous: " << prev << "\n";
	delete pprev;
}

static void doRemove(StringCache &cache) {
	std::cout << "Key: ";
	std::string key;
	std::cin >> key;
	std::string *pvalue = cache.remove(key);
	std::string value = pvalue == NULL ? std::string("-null-") : *pvalue;
	std::cout << "Previous: " << value << "\n";
	delete pvalue;
}

int main(int argc, char** args) {
	if (argc < 4) {
		std::cerr << "Usage: itest host port cache\n";
		return -1;
	}
	ConfigurationBuilder builder;
	builder.addServer().host(args[1]).port(atoi(args[2]));
	try {
		RemoteCacheManager manager(builder.build(), true);
		StringCache cache = manager.getCache<std::string, std::string>(args[3], true);
		bool printHelp = true;
		for (;;) {
			if (printHelp) {
				std::cout << "Type command: [g]et | [p]ut | [r]emove | print [s]ervers | [q]uit\n";
			}
			printHelp = true;
			int c = getchar();
			switch (c) {
			case 'q': return 0;
			case 'g': doGet(cache); break;
			case 'p': doPut(cache); break;
			case 'r': doRemove(cache); break;
			case 's': printServers(manager); break;
			case '\n': printHelp = false; break;
			default:
				std::cerr << "Invalid command '" << (char) c << "'\n";
				break;
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "Failed with error: " << e.what() << "\n";
		return 1;
	}
	return 0;
}
