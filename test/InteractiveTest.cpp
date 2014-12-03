#include <cstdio>
#include <cstdlib>
#include <vector>
#include "infinispan/hotrod/ConfigurationBuilder.h"
#include "infinispan/hotrod/RemoteCacheManager.h"

using namespace infinispan::hotrod;

typedef RemoteCache<std::string, std::string> StringCache;

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
	std::cout << "Using " << Version::getProtocolVersion() << " (version " << Version::getVersion() << ")\n";
	ConfigurationBuilder builder;
	builder.addServer().host(args[1]).port(atoi(args[2]));
	try {
		RemoteCacheManager manager(builder.build(), true);

		StringCache cache = manager.getCache<std::string, std::string>(args[3], true);
		bool printHelp = true;
		for (;;) {
			if (printHelp) {
				std::cout << "Type command: [g]et | [p]ut | [r]emove | [c]hange cache (currently: " << cache.getName() << ") | [q]uit\n";
			}
			printHelp = true;
			int c = getchar();
			switch (c) {
			case 'q': return 0;
			case 'g': doGet(cache); break;
			case 'p': doPut(cache); break;
			case 'r': doRemove(cache); break;
            case 'c': {
                std::string cacheName;
                std::cout << "Cache: ";
                std::cin >> cacheName;
                try {
                    cache = manager.getCache<std::string, std::string>(cacheName, true);
                } catch (Exception e) {
                    std::cerr << "Failed: " << e.what() << std::endl;
                }
                break;
            }
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
