/* test jni api version */

package org.infinispan.client.hotrod;

import java.io.IOException;
import java.util.HashMap;
import org.infinispan.marshall.Marshaller;

import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.jni.Hotrod;
import org.infinispan.client.hotrod.jni.RemoteCache_jb_jb;
import org.infinispan.client.hotrod.jni.RemoteCache_str_str;
import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.JniHelper;

public class RemoteCacheImpl<K, V> implements RemoteCache<K, V> {

    private Marshaller marshaller;
    private RemoteCache_jb_jb jniRemoteCache;
    private RemoteCache_str_str nativeStrCache;

    RemoteCacheImpl(RemoteCacheManager manager, String name) {
		this.marshaller = manager.getMarshaller();
		if (name.equals("native")) {
		    this.nativeStrCache = Hotrod.getStrStrCache(manager.getJniManager());
		} else { 
		    this.jniRemoteCache = Hotrod.getJniRelayCache(manager.getJniManager());
		}
    }
    
    
    
    public V put(K k, V v) {
	if (nativeStrCache != null) {
//	    nativeStrCache.put((String) k, (String) v);
	    return null;
	}
	RelayBytes krb = new RelayBytes();
	RelayBytes vrb = new RelayBytes();
	try {
	    byte[] kbytes = marshaller.objectToByteBuffer(k);
	    byte[] vbytes = marshaller.objectToByteBuffer(v);
	    try {
		JniHelper.setJvmBytes(krb, kbytes);
		JniHelper.setJvmBytes(vrb, vbytes);
		jniRemoteCache.put(krb, vrb);
//	    RelayBytes vrb2 = jniRemoteCache.put(krb, vrb);
//	    V v2 = (V) marshaller.objectFromByteBuffer(kbytes);
	    } finally {
		JniHelper.releaseJvmBytes(krb, kbytes);
		JniHelper.releaseJvmBytes(vrb, vbytes);
	    }
	} catch (IOException ioe) {
            System.out.println("oops1 action?");
	} catch (InterruptedException ie) {
	    System.out.println("oops2 action?");
	}
	return null;
    }

    public V get(K k) {
	V result = null;
	if (nativeStrCache != null) {
//	    return nativeStrCache.get(k);
	}
	if (jniRemoteCache != null) { 
	    byte[] kbytes = null;
	    try {
		kbytes = marshaller.objectToByteBuffer(k);
	    } catch (IOException ioe) {
		System.out.println("oops1 action?");
	    } catch (InterruptedException ie) {
		System.out.println("oops2 action?");
	    }

	    RelayBytes krb = new RelayBytes();
	    JniHelper.setJvmBytes(krb, kbytes);
	    RelayBytes vrb = jniRemoteCache.get(krb);
	    if (vrb != null) {
		byte[] jcopy = new byte[(int) vrb.getLength()];
		JniHelper.readNative(vrb, jcopy);
		try {
		    result = (V) marshaller.objectFromByteBuffer(jcopy);
		} catch (IOException ioe) {
		    System.out.println("oops1 action?");
		} catch (ClassNotFoundException nfee) {
		    System.out.println("oops2 action?");
		}
	    }
	    JniHelper.dispose(vrb);
	}

	return result;
    }
}
