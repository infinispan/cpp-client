/* test jni api version */

package org.infinispan.client.hotrod.impl;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import org.infinispan.commons.marshall.Marshaller;

import org.infinispan.client.hotrod.MetadataValue;
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.VersionedValue;
import org.infinispan.client.hotrod.impl.MetadataValueImpl;
import org.infinispan.client.hotrod.impl.VersionedValueImpl;
import org.infinispan.client.hotrod.jni.Hotrod;
import org.infinispan.client.hotrod.jni.JniHelper;
import org.infinispan.client.hotrod.jni.MapReturn;
import org.infinispan.client.hotrod.jni.MetadataPairReturn;
import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.RemoteCache_jb_jb;
import org.infinispan.client.hotrod.jni.RemoteCache_str_str;
import org.infinispan.client.hotrod.jni.VectorReturn;
import org.infinispan.client.hotrod.jni.VersionPairReturn;

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

    public VersionedValue<V> getVersioned(K k) {
        if (jniRemoteCache == null) {
            return null;
        }

        VersionPairReturn versionPair = null;
        RelayBytes krb = new RelayBytes();
        try {
            byte[] kbytes = marshaller.objectToByteBuffer(k);
            try {
                JniHelper.setJvmBytes(krb, kbytes);
                versionPair = jniRemoteCache.getWithVersion(krb);
                if (versionPair == null) {
                    return null;
                }
            } finally {
                JniHelper.releaseJvmBytes(krb, kbytes);
            }
        } catch (IOException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        } catch (InterruptedException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        }

        if (versionPair.getFirst() == null || Hotrod.isNull(versionPair.getFirst())) {
            return null;
        }

        RelayBytes vrb = Hotrod.dereference(versionPair.getFirst());
        if (vrb == null) {
            return null;
        }

        V value = extractValue(vrb);
        JniHelper.dispose(vrb);

        return new VersionedValueImpl(versionPair.getSecond().getVersion(), value);
    }

    public MetadataValue<V> getWithMetadata(K k) {
        if (jniRemoteCache == null) {
            return null;
        }

        MetadataPairReturn metadataPair = null;
        RelayBytes krb = new RelayBytes();
        try {
            byte[] kbytes = marshaller.objectToByteBuffer(k);
            try {
                JniHelper.setJvmBytes(krb, kbytes);
                metadataPair = jniRemoteCache.getWithMetadata(krb);
                if (metadataPair == null) {
                    return null;
                }
            } finally {
                JniHelper.releaseJvmBytes(krb, kbytes);
            }
        } catch (IOException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        } catch (InterruptedException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        }

        if (metadataPair.getFirst() == null || Hotrod.isNull(metadataPair.getFirst())) {
            return null;
        }

        RelayBytes vrb = Hotrod.dereference(metadataPair.getFirst());
        if (vrb == null) {
            return null;
        }

        V value = extractValue(vrb);
        JniHelper.dispose(vrb);

        org.infinispan.client.hotrod.jni.MetadataValue metadata = metadataPair.getSecond();
        return new MetadataValueImpl(metadata.getCreated(),
                                     (int) metadata.getLifespan(),
                                     metadata.getLastUsed(),
                                     (int) metadata.getMaxIdle(),
                                     metadata.getVersion(),
                                     value);
    }

    public Map<K, V> getBulk(int size) {
        if (jniRemoteCache == null) {
            return null;
        }

        MapReturn mapReturn = jniRemoteCache.getBulk(size);
        VectorReturn vectorReturn = Hotrod.keySet(mapReturn);

        Map<K, V> result = new HashMap<K, V>();
        for (int i = 0; i < vectorReturn.size(); i++) {
            RelayBytes krb = Hotrod.dereference(vectorReturn.get(i));
            RelayBytes vrb = Hotrod.dereference(mapReturn.get(vectorReturn.get(i)));

            result.put(extractKey(krb), extractValue(vrb));

            JniHelper.dispose(krb);
            JniHelper.dispose(vrb);
        }

        return result;
    }

    private K extractKey(RelayBytes relayBytes) {
        K key = null;
        byte[] jcopy = new byte[(int) relayBytes.getLength()];
        JniHelper.readNative(relayBytes, jcopy);
        try {
            key = (K) marshaller.objectFromByteBuffer(jcopy);
            if (key == null) {
                return null;
            }
        } catch (IOException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        }
        return key;
    }

    private V extractValue(RelayBytes relayBytes) {
        V value = null;
        byte[] jcopy = new byte[(int) relayBytes.getLength()];
        JniHelper.readNative(relayBytes, jcopy);
        try {
            value = (V) marshaller.objectFromByteBuffer(jcopy);
            if (value == null) {
                return null;
            }
        } catch (IOException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            System.out.println("Marshall error");
            e.printStackTrace();
        }
        return value;
    }
}

