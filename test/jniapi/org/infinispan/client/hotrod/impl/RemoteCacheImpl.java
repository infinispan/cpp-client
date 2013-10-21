/* test jni api version */

package org.infinispan.client.hotrod.impl;

import java.io.IOException;
import java.util.HashMap;
import org.infinispan.commons.marshall.Marshaller;

import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.jni.Hotrod;
import org.infinispan.client.hotrod.jni.RemoteCache_jb_jb;
import org.infinispan.client.hotrod.jni.RemoteCache_str_str;
import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.JniHelper;

public class RemoteCacheImpl<K, V> implements RemoteCache<K, V> {

    private Marshaller marshaller;
    private RemoteCache_jb_jb jniRemoteCache;
    private RemoteCache_str_str nativeStrCache;

    public RemoteCacheImpl(RemoteCacheManager manager, String name) {
		this.marshaller = manager.getMarshaller();
		if (name.equals("native")) {
		    this.nativeStrCache = Hotrod.getStrStrCache(manager.getJniManager());
		} else if(name == null) { 
		    this.jniRemoteCache = Hotrod.getJniRelayCache(manager.getJniManager());
		} else {
			this.jniRemoteCache = Hotrod.getJniRelayNamedCache(manager.getJniManager(), name);
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
		    } finally {
				JniHelper.releaseJvmBytes(krb, kbytes);
				JniHelper.releaseJvmBytes(vrb, vbytes);
		    }
		} catch (IOException e) {
			System.out.println("Marshall error");
			e.printStackTrace();
		} catch (InterruptedException e) {
			System.out.println("Marshall error");
			e.printStackTrace();
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
	    } catch (IOException e) {
	    	System.out.println("Marshall error");
    		e.printStackTrace();
	    } catch (InterruptedException e) {
	    	System.out.println("Marshall error");
    		e.printStackTrace();
	    }

	    RelayBytes krb = new RelayBytes();
	    JniHelper.setJvmBytes(krb, kbytes);
	    RelayBytes vrb = jniRemoteCache.get(krb);
	    if (vrb != null) {
			byte[] jcopy = new byte[(int) vrb.getLength()];
			JniHelper.readNative(vrb, jcopy);
			try {
			    result = (V) marshaller.objectFromByteBuffer(jcopy);
			} catch (IOException e) {
				System.out.println("Unmarshall error");
	    		e.printStackTrace();
			} catch (ClassNotFoundException e) {
				System.out.println("Unmarshall error");
	    		e.printStackTrace();
			}
	    }
	    JniHelper.dispose(vrb);
	}

	return result;
    }
    
    public boolean containsKey(K k) {
    	
    	if(jniRemoteCache != null) {
    		
    		byte[] kbytes = null;
    	    try {
    	    	kbytes = marshaller.objectToByteBuffer(k);
    	    } catch (IOException e) {
    	    	System.out.println("Marshall error");
	    		e.printStackTrace();
    	    } catch (InterruptedException e) {
    	    	System.out.println("Marshall error");
	    		e.printStackTrace();
    	    }

    	    RelayBytes krb = new RelayBytes();
    	    JniHelper.setJvmBytes(krb, kbytes);
    	    
    	    return jniRemoteCache.containsKey(krb);
    	}
    	
    	return false;
    }
    
    public V remove(K k) {
    	V result = null;
    	if (jniRemoteCache != null) { 
    	    byte[] kbytes = null;
    	    try {
    	    	kbytes = marshaller.objectToByteBuffer(k);
    	    } catch (IOException e) {
	    		System.out.println("Marshall error");
	    		e.printStackTrace();
    	    } catch (InterruptedException e) {
    	    	System.out.println("Marshall error");
        		e.printStackTrace();
    	    }

    	    RelayBytes krb = new RelayBytes();
    	    JniHelper.setJvmBytes(krb, kbytes);
    	    RelayBytes vrb = jniRemoteCache.remove(krb);
    	    if (vrb != null) {
	    		byte[] jcopy = new byte[(int) vrb.getLength()];
	    		JniHelper.readNative(vrb, jcopy);
	    		try {
	    		    result = (V) marshaller.objectFromByteBuffer(jcopy);
	    		} catch (IOException e) {
	    			System.out.println("Marshall error");
	        		e.printStackTrace();
	    		} catch (ClassNotFoundException e) {
	    		    System.out.println("Marshall error");
	        		e.printStackTrace();
	    		}
    	    }
    	    JniHelper.dispose(vrb);
    	}
    	
    	return result;
    }
    
    public V replace(K k, V v) {
    	V result = null;
    	if (jniRemoteCache != null) { 

        	RelayBytes krb = new RelayBytes();
    		RelayBytes vrb = new RelayBytes();
    		RelayBytes resrb = new RelayBytes();
    		
    		try {
    			byte[] kbytes = marshaller.objectToByteBuffer(k);
    			byte[] vbytes = marshaller.objectToByteBuffer(v);
    		    
    		     	try {
    					JniHelper.setJvmBytes(krb, kbytes);
    					JniHelper.setJvmBytes(vrb, vbytes);
    					resrb = jniRemoteCache.replace(krb, vrb);
    					
    					if (resrb != null) {
        		    		byte[] jcopy = new byte[(int) resrb.getLength()];
        		    		JniHelper.readNative(resrb, jcopy);
        		    		try {
        		    		    result = (V) marshaller.objectFromByteBuffer(jcopy);
        		    		} catch (IOException e) {
        		    			System.out.println("Marshall error");
        		        		e.printStackTrace();
        		    		} catch (ClassNotFoundException e) {
        		    		    System.out.println("Marshall error");
        		        		e.printStackTrace();
        		    		}
        		    		JniHelper.dispose(resrb);
        	    	    }
        	    	    
    			    } finally {
    					JniHelper.releaseJvmBytes(krb, kbytes);
    					JniHelper.releaseJvmBytes(vrb, vbytes);
    			    }
    		
    		} catch (IOException e) {
    			System.out.println("Marshall error");
    			e.printStackTrace();
    		} catch (InterruptedException e) {
    			System.out.println("Marshall error");
    			e.printStackTrace();
    		}
    		
    		return result;
        }
    	
    	return null;
    }
    
    public V putIfAbsent(K k, V v) {
    	
    	RelayBytes krb = new RelayBytes();
		RelayBytes vrb = new RelayBytes();
		try {
		    byte[] kbytes = marshaller.objectToByteBuffer(k);
		    byte[] vbytes = marshaller.objectToByteBuffer(v);
		    try {
				JniHelper.setJvmBytes(krb, kbytes);
				JniHelper.setJvmBytes(vrb, vbytes);
				jniRemoteCache.putIfAbsent(krb, vrb);
		    } finally {
				JniHelper.releaseJvmBytes(krb, kbytes);
				JniHelper.releaseJvmBytes(vrb, vbytes);
		    }
		} catch (IOException e) {
			System.out.println("Marshall error");
			e.printStackTrace();
		} catch (InterruptedException e) {
			System.out.println("Marshall error");
			e.printStackTrace();
		}
		
		return null;
    }
    
    public void clear() {
    	if (jniRemoteCache != null) { 
	    	if(jniRemoteCache != null) {
	    		jniRemoteCache.clear();
	    	}
    	}
    }
}

