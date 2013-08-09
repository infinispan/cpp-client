%module Hotrod

%include "arrays_java.i"
%include "typemaps.i"
%include "various.i"
%include "std_string.i"
%include "std_map.i"


%{
#include <hotrod/impl/configuration/ConnectionPoolConfiguration.h>
#include <hotrod/impl/configuration/ServerConfiguration.h>
#include <hotrod/impl/configuration/SslConfiguration.h>
#include <hotrod/impl/configuration/Configuration.h>
#include <hotrod/impl/configuration/ServerConfigurationBuilder.h>
#include <hotrod/impl/configuration/ConfigurationBuilder.h>
#include <infinispan/hotrod/MetadataValue.h>
#include <infinispan/hotrod/Flag.h>
#include <infinispan/hotrod/RemoteCacheManager.h>
#include <infinispan/hotrod/RemoteCache.h>
#include <infinispan/hotrod/ScopedBuffer.h>
#include <infinispan/hotrod/Marshaller.h>

//ZZZ
#include <iostream>
%}

//I want use java.util.Map, not obscure SWIG types
%typemap(jstype) std::map<std::string, std::string> "java.util.Map<String,String>"
%typemap(javain,pre="    MapType temp$javainput = $javaclassname.convertMap($javainput);",pgcppname="temp$javainput") std::map<std::string, std::string> "$javaclassname.getCPtr(temp$javainput)"
%typemap(javacode) std::map<std::string, std::string> %{
  static $javaclassname convertMap(java.util.Map<String,String> in) {
    $javaclassname out = new $javaclassname();
    for (java.util.Map.Entry<String, String> entry : in.entrySet()) {
      out.set(entry.getKey(), entry.getValue());      
    }
    return out;
  }    
%}

%template(MapType) std::map<std::string, std::string>;

%include "infinispan/hotrod/ImportExport.h"
%include "infinispan/hotrod/Handle.h"

%template(HandleRemoteCacheManagerImpl) infinispan::hotrod::Handle<infinispan::hotrod::RemoteCacheManagerImpl>;

%include "hotrod/impl/configuration/Builder.h"
%include "hotrod/impl/configuration/ConnectionPoolConfiguration.h"
%include "hotrod/impl/configuration/ServerConfiguration.h"
%include "hotrod/impl/configuration/SslConfiguration.h"
%include "hotrod/impl/configuration/Configuration.h"

%template(Builder_conf) infinispan::hotrod::Builder<infinispan::hotrod::Configuration>;
%template(Builder_serverconf) infinispan::hotrod::Builder<infinispan::hotrod::ServerConfiguration>;

%include "hotrod/impl/configuration/ServerConfigurationBuilder.h"
%include "hotrod/impl/configuration/ConnectionPoolConfigurationBuilder.h"
%include "hotrod/impl/configuration/ConfigurationBuilder.h"
%include "infinispan/hotrod/MetadataValue.h"
%include "infinispan/hotrod/Flag.h"
%include "infinispan/hotrod/RemoteCacheManager.h"
%include "infinispan/hotrod/RemoteCache.h"
%include "infinispan/hotrod/Marshaller.h"



%template(RemoteCache_str_str) infinispan::hotrod::RemoteCache<std::string, std::string>;

// One class to pass bytes between the JVM and the native classes
// For the exclusive use of Relay marshalling

%ignore RelayBytes::setNative;
%ignore RelayBytes::setJvm;
%ignore RelayBytes::getBytes;
%ignore RelayBytes::getJarray;

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

// our mechanism for RemoteCache<byte[], byte[]> from the java side
%template(RemoteCache_jb_jb) infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes>;

# create a do-nothing marshaller works with the pre-marshalled data

%{
using infinispan::hotrod::ScopedBuffer;

class RelayMarshallerHelper {
  public:
    static void noRelease(ScopedBuffer*) { /* nothing allocated, nothing to release */ }
};

class RelayMarshaller: public infinispan::hotrod::Marshaller<RelayBytes> {
  public:
    void marshall(const RelayBytes& jb, ScopedBuffer& b) {
	if (jb.getLength() == 0) {
	    return;
	}
        b.set(jb.getBytes(), jb.getLength(), &RelayMarshallerHelper::noRelease);
    }
    RelayBytes* unmarshall(const ScopedBuffer& sbuf) {
	RelayBytes* rbytes = new RelayBytes();
	size_t size = sbuf.getLength();
	char *bytes = new char[size];
	memcpy(bytes, sbuf.getBytes(), size);
	rbytes->setNative(bytes, size);
	return rbytes;
    }
};

%}


infinispan::hotrod::RemoteCache<RelayBytes, RelayBytes>* getJniRelayCache(infinispan::hotrod::RemoteCacheManager& mgr);
infinispan::hotrod::RemoteCache<std::string, std::string>* getStrStrCache(infinispan::hotrod::RemoteCacheManager& mgr);

%{
using infinispan::hotrod::Marshaller;
using infinispan::hotrod::RemoteCache;
using infinispan::hotrod::RemoteCacheManager;

// Allow a heap allocated instance of RemoteCache<RelayBytes,RelayBytes> 
// for JNI purposes

class RelayCacheBlob {
  public:
     RelayCacheBlob(RemoteCacheManager &m) : 
       keyMarshaller(new RelayMarshaller()),
       valueMarshaller(new RelayMarshaller()),
       cache(m.getCache<RelayBytes, RelayBytes>(keyMarshaller, valueMarshaller)) {}

     RemoteCache<RelayBytes, RelayBytes> *getCache() {
	 return &cache;
     }

  private:
     HR_SHARED_PTR<Marshaller<RelayBytes> > keyMarshaller;
     HR_SHARED_PTR<Marshaller<RelayBytes> > valueMarshaller;
     RemoteCache<RelayBytes, RelayBytes> cache;
};


RemoteCache<RelayBytes, RelayBytes>* getJniRelayCache(RemoteCacheManager& mgr) {
    RelayCacheBlob *p = new RelayCacheBlob(mgr); // leak (test only): TODO: blob cleanup
    return p->getCache();
}


class StrCacheBlob {
  public:
     StrCacheBlob(RemoteCacheManager &m) : 
       cache(m.getCache<std::string, std::string>()) {}

     RemoteCache<std::string, std::string> *getCache() {
	 return &cache;
     }

  private:
     RemoteCache<std::string, std::string> cache;
};


RemoteCache<std::string, std::string>* getStrStrCache(RemoteCacheManager& mgr) {
    StrCacheBlob *p = new StrCacheBlob(mgr); // leak (test only): TODO: blob cleanup
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
    arg1->setJvm((char *) jenv->GetByteArrayElements(jarg2, 0),
		 (size_t) jenv->GetArrayLength(jarg2), jarg2);
}


SWIGEXPORT void JNICALL Java_org_infinispan_client_hotrod_jni_HotrodJNI2_releaseJvmBytes(JNIEnv *jenv, jclass jcls, jlong jarg1, jbyteArray jarg2) {
    (void) jcls;
    RelayBytes *arg1 = (RelayBytes *) jarg1;
    if (arg1->getBytes())
	jenv->ReleaseByteArrayElements(arg1->getJarray(), (jbyte *) arg1->getBytes(), 0);
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

