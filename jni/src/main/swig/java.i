%module Hotrod

%include "arrays_java.i"
%include "typemaps.i"
%include "various.i"
%include "std_string.i"
%include "std_map.i"
%include "std_set.i"
%include "std_vector.i"
%include "stdint.i"

//#define SWIG_SHARED_PTR_NAMESPACE std
//#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include "std_shared_ptr.i"


%include "std_pair.i"

//#define std::shared_ptr std::shared_ptr

%{
#include <infinispan/hotrod/defs.h>
#include <infinispan/hotrod/ConnectionPoolConfiguration.h>
#include <infinispan/hotrod/exceptions.h>
#include <infinispan/hotrod/InetSocketAddress.h>
#include <infinispan/hotrod/FailOverRequestBalancingStrategy.h>
#include <infinispan/hotrod/ServerConfiguration.h>
#include <infinispan/hotrod/SslConfiguration.h>
#include <infinispan/hotrod/Configuration.h>
#include <infinispan/hotrod/ServerConfigurationBuilder.h>
#include <infinispan/hotrod/ConfigurationBuilder.h>
#include <infinispan/hotrod/MetadataValue.h>
#include <infinispan/hotrod/VersionedValue.h>
#include <infinispan/hotrod/Flag.h>
#include <infinispan/hotrod/TimeUnit.h>
#include <infinispan/hotrod/RemoteCacheManager.h>
#include <infinispan/hotrod/RemoteCache.h>
#include <infinispan/hotrod/Marshaller.h>
#include <infinispan/hotrod/CacheTopologyInfo.h>

#include <iostream>
using namespace infinispan::hotrod;
%}

%template(MapType) std::map<std::string, std::string>;
%template(SegPerServer) std::map<infinispan::hotrod::transport::InetSocketAddress, std::vector<int> >;

%include "infinispan/hotrod/defs.h"
%include "infinispan/hotrod/ImportExport.h"
%include "infinispan/hotrod/FailOverRequestBalancingStrategy.h"
%include "infinispan/hotrod/ConnectionPoolConfiguration.h"
%include "infinispan/hotrod/ServerConfiguration.h"
%include "infinispan/hotrod/SslConfiguration.h"

%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_10;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_11;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_12;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_13;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_20;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_21;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_22;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_23;
%immutable infinispan::hotrod::Configuration::PROTOCOL_VERSION_24;


%include "infinispan/hotrod/Configuration.h"


%ignore infinispan::hotrod::transport::InetSocketAddress::operator==;
%ignore infinispan::hotrod::transport::InetSocketAddress::operator=;
%ignore operator<<;
%ignore infinispan::hotrod::transport::operator<<;
%ignore infinispan::hotrod::transport::InetSocketAddress::operator<;
%ignore infinispan::hotrod::RemoteCache::operator=;

%include "infinispan/hotrod/InetSocketAddress.h"
%include "infinispan/hotrod/FailOverRequestBalancingStrategy.h"
%include "infinispan/hotrod/SslConfigurationBuilder.h"
%include "infinispan/hotrod/ServerConfigurationBuilder.h"
%include "infinispan/hotrod/ConnectionPoolConfigurationBuilder.h"
%include "infinispan/hotrod/ConfigurationBuilder.h"
%include "infinispan/hotrod/VersionedValue.h"
%include "infinispan/hotrod/MetadataValue.h"
%include "infinispan/hotrod/Flag.h"
%include "infinispan/hotrod/TimeUnit.h"
%include "infinispan/hotrod/RemoteCacheManager.h"
%include "infinispan/hotrod/RemoteCache.h"
%include "infinispan/hotrod/Marshaller.h"
%include "infinispan/hotrod/CacheTopologyInfo.h"

%include "infinispan/hotrod/exceptions.h"

%exception {
    try {
        $action
    } catch (const infinispan::hotrod::InvalidResponseException& e) {
      jclass clazz = jenv->FindClass("org/infinispan/client/hotrod/exceptions/InvalidResponseException");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const infinispan::hotrod::RemoteCacheManagerNotStartedException& e) {
      jclass clazz = jenv->FindClass("org/infinispan/client/hotrod/exceptions/RemoteCacheManagerNotStartedException");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const infinispan::hotrod::UnsupportedOperationException& e) {
      jclass clazz = jenv->FindClass("java/lang/UnsupportedOperationException");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const infinispan::hotrod::HotRodClientException& e) {
      jclass clazz = jenv->FindClass("org/infinispan/client/hotrod/exceptions/HotRodClientException");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const infinispan::hotrod::Exception& e) {
      jclass clazz = jenv->FindClass("java/lang/Exception");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const std::runtime_error& e) {
      SWIG_exception(SWIG_RuntimeError, e.what());
      return $null;
    } catch (const std::exception& e) {
      jclass clazz = jenv->FindClass("java/lang/Exception");
      jenv->ThrowNew(clazz, e.what());
      return $null;
    } catch (const std::exception *e) {
      jclass clazz = jenv->FindClass("java/lang/Exception");
      jenv->ThrowNew(clazz, e->what());
      return $null;
    } catch (...) {
      SWIG_exception(SWIG_UnknownError, "C++ unknown exception thrown");
      return $null;
    }
}

// One class to pass bytes between the JVM and the native classes
// For the exclusive use of Relay marshalling

%ignore RelayBytes::setNative;
%ignore RelayBytes::setJvm;
%ignore RelayBytes::getBytes;
%ignore RelayBytes::getJarray;

%ignore getAsync;
%ignore putAsync;
%ignore goAsync;
%ignore putAllAsync;
%ignore replaceWithVersionAsync;
%ignore removeWithVersionAsync;
%ignore putIfAbsentAsync;
%ignore clearAsync;
%ignore removeAsync;
%ignore replaceAsync;

//%shared_ptr(RelayShrPointer)

%inline %{

class RelayBytes {
  public:
    void setNative(char *b, size_t l) {bytes = b; length = l;}
    void setJvm(char *b, size_t l, jbyteArray a) {bytes = b; length = l; jarray = a; }
    char *getBytes () const {return bytes;}
    size_t getLength () const {return length; }
    jbyteArray getJarray() const { return jarray; }
    bool isNull() const { return !bytes; }
  private:
    char *bytes;
    size_t length;
    jbyteArray jarray;
};

%}

//%template(RelayShrPointer) std::shared_ptr<RelayBytes>;
%template(MetadataPairReturn) std::pair<std::shared_ptr<RelayBytes>, infinispan::hotrod::MetadataValue>;
%template(VersionPairReturn) std::pair<std::shared_ptr<RelayBytes>, infinispan::hotrod::VersionedValue>;
%template(MapReturn) std::map<std::shared_ptr<RelayBytes>, std::shared_ptr<RelayBytes> >;
%template(SetReturn) std::set<std::shared_ptr<RelayBytes> >;
%template(VectorReturn) std::vector<std::shared_ptr<RelayBytes> >;
%template(StringVectorReturn) std::vector<std::string>;
%template(IntegerVectorReturn) std::vector<int>;
%template(InetSocketAddressvectorReturn) std::vector<infinispan::hotrod::transport::InetSocketAddress>;
%template(ServerConfigurationBuilderVector) std::vector<infinispan::hotrod::ServerConfigurationBuilder>;
%template(ServerConfigurationVector) std::vector<infinispan::hotrod::ServerConfiguration>;
%template(ServerConfigurationMap) std::map<std::string,std::vector<infinispan::hotrod::ServerConfiguration> >;

%inline %{
 bool isNull(std::shared_ptr<RelayBytes> ptr) {
     return !ptr;
 }

 RelayBytes dereference(std::shared_ptr<RelayBytes> ptr) {
     return *ptr;
 }


std::vector<infinispan::hotrod::transport::InetSocketAddress> keySet(std::map<infinispan::hotrod::transport::InetSocketAddress, std::vector<int> > map)
{
     std::vector<infinispan::hotrod::transport::InetSocketAddress > result;
     for (std::map<infinispan::hotrod::transport::InetSocketAddress, std::vector<int> >::iterator it = map.begin(); it != map.end(); ++it)
         result.push_back(it->first);
     return result;
}
 
 std::vector<std::shared_ptr<RelayBytes> > keySet(std::map<std::shared_ptr<RelayBytes>, std::shared_ptr<RelayBytes> > map) {
     std::vector<std::shared_ptr<RelayBytes> > result;
     for (std::map<std::shared_ptr<RelayBytes>, std::shared_ptr<RelayBytes> >::iterator it = map.begin(); it != map.end(); ++it)
         result.push_back(it->first);
     return result;
 }
 
 std::vector<std::shared_ptr<RelayBytes> > keys(std::set<std::shared_ptr<RelayBytes> > set) {
     std::vector<std::shared_ptr<RelayBytes> > result;
     for (std::set<std::shared_ptr<RelayBytes> >::iterator it = set.begin(); it != set.end(); ++it)
         result.push_back(*it);
     return result;
 }
 
 std::vector<std::string > keySet(std::map<std::string, std::string > map) {
     std::vector<std::string > result;
     for (std::map<std::string, std::string >::iterator it = map.begin(); it != map.end(); ++it)
         result.push_back(it->first);
     return result;
 }
%}

// our mechanism for RemoteCache<byte[], byte[]> from the java side
%template(RemoteCache_jb_jb) infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes>;

// create a do-nothing marshaller works with the pre-marshalled data

%{

class RelayMarshallerHelper {
  public:
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }
};

class RelayMarshaller: public infinispan::hotrod::Marshaller<RelayBytes> {
  public:
    void marshall(const RelayBytes& jb, std::vector<char>& b) {
	if (jb.getLength() == 0) {
	    return;
	}
        b.assign(jb.getBytes(), jb.getBytes()+jb.getLength());
    }
    RelayBytes* unmarshall(const std::vector<char>& sbuf) {
	RelayBytes* rbytes = new RelayBytes();
	size_t size = sbuf.size();
	char *bytes = new char[size];
	memcpy(bytes, sbuf.data(), size);
	rbytes->setNative(bytes, size);
	return rbytes;
    }
};

%}

infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes>* getJniRelayCache(infinispan::hotrod::RemoteCacheManager& mgr, bool forceReturnValue);
infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes>* getJniRelayNamedCache(infinispan::hotrod::RemoteCacheManager& mgr, std::string name, bool forceReturnValue);

%{
using infinispan::hotrod::Marshaller;
using infinispan::hotrod::RemoteCache;
using infinispan::hotrod::RemoteCacheManager;

// Allow a heap allocated instance of RemoteCache<RelayBytes,RelayBytes> 
// for JNI purposes


class RelayCacheBlob {
  public:
     RelayCacheBlob(RemoteCacheManager &m, bool forceReturnValue) : 
       cache(m.getCache<RelayBytes, RelayBytes>(new RelayMarshaller(), &Marshaller<RelayBytes>::destroy,
                                                new RelayMarshaller(), &Marshaller<RelayBytes>::destroy, forceReturnValue)) {}

     RelayCacheBlob(RemoteCacheManager &m, std::string &name, bool forceReturnValue) : 
       cache(m.getCache<RelayBytes, RelayBytes>(new RelayMarshaller(), &Marshaller<RelayBytes>::destroy,
                                                new RelayMarshaller(), &Marshaller<RelayBytes>::destroy, name.c_str(), forceReturnValue)) {}

     RemoteCache<RelayBytes, RelayBytes> *getCache() {
       return &cache;
     }

  private:
     RemoteCache<RelayBytes, RelayBytes> cache;
};


RemoteCache<RelayBytes, RelayBytes>* getJniRelayCache(RemoteCacheManager& mgr, bool forceReturnValue) {
    RelayCacheBlob *p = new RelayCacheBlob(mgr, forceReturnValue); // leak (test only): TODO: blob cleanup
    return p->getCache();
}

RemoteCache<RelayBytes, RelayBytes>* getJniRelayNamedCache(RemoteCacheManager& mgr, std::string name, bool forceReturnValue) {
    RelayCacheBlob *p = new RelayCacheBlob(mgr, name, forceReturnValue); // leak (test only): TODO: blob cleanup
    return p->getCache();
}

%}



%{

extern "C" {
SWIGEXPORT jlong JNICALL Java_org_infinispan_client_hotrod_jni_HotrodJNI2_new_1JniTest(JNIEnv *jenv, jclass jcls) {
  jlong jresult = 0 ;
  
  (void)jenv;
  (void)jcls;
  jresult = 77;
  return jresult;
}

// The following hand coded JNICALL functions are modified swig stubs
// TODO: replace with proper typemaps.  Bonus points for saving a memcpy.
// ZZZ more doc

// setJvmBytes() get a pointer to the jbyteArray without copying

SWIGEXPORT void JNICALL Java_org_infinispan_client_hotrod_jni_HotrodJNI2_setJvmBytes(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    (void) jcls;
    RelayBytes *arg1 = (RelayBytes *) jarg1;
    jbyteArray jarray = (jbyteArray) jenv->NewGlobalRef(jarg2);
    arg1->setJvm((char *) jenv->GetByteArrayElements(jarg2, 0),
		 (size_t) jenv->GetArrayLength(jarg2), jarray);
}


SWIGEXPORT void JNICALL Java_org_infinispan_client_hotrod_jni_HotrodJNI2_releaseJvmBytes(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    (void) jcls;
    RelayBytes *arg1 = (RelayBytes *) jarg1;
    if (arg1->getBytes()) {
        jbyteArray jarray = arg1->getJarray();
        jenv->ReleaseByteArrayElements(jarray, (jbyte *) arg1->getBytes(), 0);
        jenv->DeleteGlobalRef(jarray);
    }
}


SWIGEXPORT void JNICALL Java_org_infinispan_client_hotrod_jni_HotrodJNI2_readNative(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    (void) jcls;
    RelayBytes *arg1 = (RelayBytes *) jarg1;
    jbyte *target = jenv->GetByteArrayElements(jarg2, 0);
    char *source = arg1->getBytes();
    memcpy(target, source, arg1->getLength());
    jenv->ReleaseByteArrayElements(jarg2, target, 0);
    arg1->setNative(0,0);
    delete[] source;
}

}

%}
