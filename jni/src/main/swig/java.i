%module Hotrod

%include "arrays_java.i"
%include "typemaps.i"
%include "various.i"
%include "std_string.i"
%include "std_map.i"
%include "std_set.i"
%include "std_future.i"
%include "std_vector.i"
%include "stdint.i"

//#define SWIG_SHARED_PTR_NAMESPACE std
//#define SWIG_SHARED_PTR_SUBNAMESPACE tr1
%include "std_shared_ptr.i"


%include "std_pair.i"

%include "enums.swg"
%javaconst(1);

%shared_ptr(infinispan::hotrod::RemoteCacheManagerAdmin)
//#define std::shared_ptr std::shared_ptr

%{
#include <infinispan/hotrod/defs.h>
#include <infinispan/hotrod/ConnectionPoolConfiguration.h>
#include <infinispan/hotrod/exceptions.h>
#include <infinispan/hotrod/InetSocketAddress.h>
#include <infinispan/hotrod/FailOverRequestBalancingStrategy.h>
#include <infinispan/hotrod/EventMarshaller.h>
#include <infinispan/hotrod/JBasicEventMarshaller.h>
#include <infinispan/hotrod/ServerConfiguration.h>
#include <infinispan/hotrod/SslConfiguration.h>
#include <infinispan/hotrod/AuthenticationConfiguration.h>
#include <infinispan/hotrod/SecurityConfiguration.h>
#include <infinispan/hotrod/NearCacheConfiguration.h>
#include <infinispan/hotrod/Configuration.h>
#include <infinispan/hotrod/ServerConfigurationBuilder.h>
#include <infinispan/hotrod/ConfigurationBuilder.h>
#include <infinispan/hotrod/MetadataValue.h>
#include <infinispan/hotrod/VersionedValue.h>
#include <infinispan/hotrod/Flag.h>
#include <infinispan/hotrod/TimeUnit.h>
#include <infinispan/hotrod/RemoteCacheManager.h>
#include <infinispan/hotrod/RemoteCacheManagerAdmin.h>
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
%include "infinispan/hotrod/EventMarshaller.h"
%include "infinispan/hotrod/JBasicEventMarshaller.h"
%include "infinispan/hotrod/ConnectionPoolConfiguration.h"
%include "infinispan/hotrod/ServerConfiguration.h"
%include "infinispan/hotrod/SslConfiguration.h"
%include "infinispan/hotrod/AuthenticationConfiguration.h"
%include "infinispan/hotrod/SecurityConfiguration.h"
%include "infinispan/hotrod/NearCacheConfiguration.h"

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
%include "infinispan/hotrod/ConfigurationChildBuilder.h"
%include "infinispan/hotrod/SslConfigurationBuilder.h"
%include "infinispan/hotrod/AuthenticationConfigurationBuilder.h"
%include "infinispan/hotrod/SecurityConfigurationBuilder.h"
%include "infinispan/hotrod/ServerConfigurationBuilder.h"
%include "infinispan/hotrod/ConnectionPoolConfigurationBuilder.h"
%include "infinispan/hotrod/ConfigurationBuilder.h"
%include "infinispan/hotrod/VersionedValue.h"
%include "infinispan/hotrod/MetadataValue.h"
%include "infinispan/hotrod/Flag.h"
%include "infinispan/hotrod/TimeUnit.h"
%include "infinispan/hotrod/RemoteCacheManager.h"
%include "infinispan/hotrod/RemoteCacheManagerAdmin.h"
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
%ignore goAsync;
%ignore JBossMarshaller;
%ignore addExecuteArg;
%ignore putExecuteScript;
%ignore addContinuousQueryListener;

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

inline bool operator< (const RelayBytes& lhs, const RelayBytes& rhs)
{
  auto res = memcmp(lhs.getBytes(), rhs.getBytes(), std::min(lhs.getLength(),rhs.getLength()));
  return (res != 0) ? res<0 : lhs.getLength()<rhs.getLength();
}
%}

%template(RelayShrPointer) std::shared_ptr<RelayBytes>;
%template(MetadataPairReturn) std::pair<std::shared_ptr<RelayBytes>, infinispan::hotrod::MetadataValue>;
%template(VersionPairReturn) std::pair<std::shared_ptr<RelayBytes>, infinispan::hotrod::VersionedValue>;
%template(MapReturn) std::map<std::shared_ptr<RelayBytes>, std::shared_ptr<RelayBytes> >;
%template(MapArg) std::map<RelayBytes, RelayBytes >;
%template(SetReturn) std::set<std::shared_ptr<RelayBytes> >;
%template(SetArgs) std::set<RelayBytes>;
%template(VectorReturn) std::vector<std::shared_ptr<RelayBytes> >;
%template(StringVectorReturn) std::vector<std::string>;
%template(IntegerVectorReturn) std::vector<int>;
%template(UCharVector) std::vector<unsigned char>;
%template(InetSocketAddressvectorReturn) std::vector<infinispan::hotrod::transport::InetSocketAddress>;
%ignore std::vector<infinispan::hotrod::ServerConfigurationBuilder>::vector(size_type);
%ignore std::vector<infinispan::hotrod::ServerConfigurationBuilder>::resize;
%template(ServerConfigurationBuilderVector) std::vector<infinispan::hotrod::ServerConfigurationBuilder>;
%template(ServerConfigurationVector) std::vector<infinispan::hotrod::ServerConfiguration>;
%template(ServerConfigurationMap) std::map<std::string,std::vector<infinispan::hotrod::ServerConfiguration> >;
%template(FutureRelayBytes) std::future<RelayBytes*>;
%template(FutureBool) std::future<bool>;
%template(FutureVoid) std::future<void>;
%template(AdminFlagSet) std::set<infinispan::hotrod::AdminFlag>;
%template(SetString) std::set<std::string>;

%feature("novaluewrapper") std::future<RelayBytes>;
%feature("novaluewrapper") std::future<RelayBytes*>;
%feature("novaluewrapper") std::future<bool>;
%feature("novaluewrapper") std::future<void>;

%typemap(out) std::future<RelayBytes*> "$result= (jlong) new std::future< RelayBytes * >((std::future< RelayBytes * > &&)$1);";
%typemap(out) std::future<bool> "$result= (jlong) new std::future< bool >((std::future< bool > &&)$1);";
%typemap(out) std::future<void> "$result= (jlong) new std::future< void >((std::future< void > &&)$1);";


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
%template(adminCreateCache_jb_jb) infinispan::hotrod::RemoteCacheManagerAdmin::createCache<RelayBytes,RelayBytes>;
%template(adminGetOrCreateCache_jb_jb) infinispan::hotrod::RemoteCacheManagerAdmin::getOrCreateCache<RelayBytes,RelayBytes>;
%template(adminCreateCacheWithXml_jb_jb) infinispan::hotrod::RemoteCacheManagerAdmin::createCacheWithXml<RelayBytes,RelayBytes>;
%template(adminGetOrCreateCacheWithXml_jb_jb) infinispan::hotrod::RemoteCacheManagerAdmin::getOrCreateCacheWithXml<RelayBytes,RelayBytes>;

// create a do-nothing marshaller works with the pre-marshalled data

%{

class RelayMarshallerHelper {
  public:
    static void noRelease(std::vector<char>*) { /* nothing allocated, nothing to release */ }
};

namespace infinispan {
namespace hotrod {
template <>
class BasicMarshaller<RelayBytes> : public infinispan::hotrod::Marshaller<RelayBytes> {
    public:
        void marshall(const RelayBytes& r, std::vector<char>& b) {
            if (r.getLength() == 0) {
                b.clear();
                return;
            }
            b.assign(r.getBytes(), r.getBytes()+r.getLength());
        }
        RelayBytes* unmarshall(const std::vector<char>& b) {
            RelayBytes* rbytes = new RelayBytes();
            size_t size = b.size();
            char *bytes = new char[size];
            memcpy(bytes, b.data(), size);
            rbytes->setNative(bytes, size);
            return rbytes;
        }
};
}}

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

%extend infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes> {
   void dispose(std::future<bool>* p) { delete p; }
   void dispose(std::future<RelayBytes*>* p) {  delete p->get(); delete p; }
   void dispose(std::future<void>* p) { delete p; }
}

%extend infinispan::hotrod::RemoteCacheManagerAdmin {

    RemoteCacheManagerAdmin& withFlags(infinispan::hotrod::AdminFlag f0,infinispan::hotrod::AdminFlag f1)
    {
        $self->flags.clear();
        $self->flags.insert(f0);
        $self->flags.insert(f1);
        return *$self;
    }
    RemoteCacheManagerAdmin& withFlags(infinispan::hotrod::AdminFlag f0, infinispan::hotrod::AdminFlag f1, infinispan::hotrod::AdminFlag f2)
    {
        $self->flags.clear();
        $self->flags.insert(f0);
        $self->flags.insert(f1);
        $self->flags.insert(f2);
        return *$self;
    }
}
