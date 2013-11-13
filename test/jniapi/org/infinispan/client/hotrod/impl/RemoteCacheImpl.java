/* test jni api version */

package org.infinispan.client.hotrod.impl;

import static org.infinispan.client.hotrod.jni.JniHelper.dispose;
import static org.infinispan.client.hotrod.jni.JniHelper.readNative;
import static org.infinispan.client.hotrod.jni.JniHelper.releaseJvmBytes;
import static org.infinispan.client.hotrod.jni.JniHelper.setJvmBytes;
import static org.infinispan.client.hotrod.jni.JniHelper.timeunitToSwig;

import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;
import java.util.concurrent.TimeUnit;

import org.infinispan.client.hotrod.Flag;
import org.infinispan.client.hotrod.MetadataValue;
import org.infinispan.client.hotrod.RemoteCache;
import org.infinispan.client.hotrod.RemoteCacheManager;
import org.infinispan.client.hotrod.VersionedValue;
import org.infinispan.client.hotrod.jni.Hotrod;
import org.infinispan.client.hotrod.jni.MapReturn;
import org.infinispan.client.hotrod.jni.MetadataPairReturn;
import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.RemoteCache_jb_jb;
import org.infinispan.client.hotrod.jni.SetReturn;
import org.infinispan.client.hotrod.jni.VectorReturn;
import org.infinispan.client.hotrod.jni.VersionPairReturn;
import org.infinispan.commons.marshall.Marshaller;

public class RemoteCacheImpl<K, V> extends RemoteCacheUnsupported<K, V> {

    private Marshaller marshaller;
    private RemoteCache_jb_jb jniRemoteCache;

    public RemoteCacheImpl(RemoteCacheManager manager, String name, boolean forceReturnValue) {
        this.marshaller = manager.getMarshaller();
        if (name == null) {
            this.jniRemoteCache = Hotrod.getJniRelayCache(manager.getJniManager(), forceReturnValue);
        } else {
            this.jniRemoteCache = Hotrod.getJniRelayNamedCache(manager.getJniManager(), name, forceReturnValue);
        }
    }

    @Override
    public String getName() {
        return jniRemoteCache.getName();
    }

    @Override
    public String getVersion() {
        return jniRemoteCache.getVersion();
    }

    @Override
    public String getProtocolVersion() {
        return jniRemoteCache.getProtocolVersion();
    }

    @Override
    public void start() {
        // Nothing to do
    }

    @Override
    public void stop() {
        // Nothing to do
    }

    @Override
    public V put(K k, V v) {
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.put(rbs[0], rbs[1]);
            }
        }, k, v);
    }

    @Override
    public V put(K k, V v, long lifespan, TimeUnit unit) {
        final long l = lifespan;
        final TimeUnit lu = unit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.put(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu));
            }
        }, k, v);
    }

    @Override
    public V put(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        final long l = lifespan;
        final TimeUnit lu = lifespanTimeUnit;
        final long m = maxIdle;
        final TimeUnit mu = maxIdleUnit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.put(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu),
                        BigInteger.valueOf(m), timeunitToSwig(mu));
            }
        }, k, v);
    }

    @Override
    public V get(K k) {
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.get(rbs[0]);
            }
        }, k);
    }

    @Override
    public boolean containsKey(K k) {
        Boolean b = relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.containsKey(rbs[0]);
            }
        }, k);
        return b;
    }

    @Override
    public boolean containsValue(V v) {
        Boolean b = relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.containsValue(rbs[0]);
            }
        }, v);
        return b;
    }

    @Override
    public V remove(K k) {
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.remove(rbs[0]);
            }
        }, k);
    }

    @Override
    public V replace(K k, V v) {
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.replace(rbs[0], rbs[1]);
            }
        }, k, v);
    }

    @Override
    public V replace(K k, V v, long lifespan, TimeUnit unit) {
        final long l = lifespan;
        final TimeUnit lu = unit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.replace(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu));
            }
        }, k, v);
    }

    @Override
    public V replace(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        final long l = lifespan;
        final TimeUnit lu = lifespanTimeUnit;
        final long m = maxIdle;
        final TimeUnit mu = maxIdleUnit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.replace(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu),
                        BigInteger.valueOf(m), timeunitToSwig(mu));
            }
        }, k, v);
    }

    @Override
    public V putIfAbsent(K k, V v) {
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.putIfAbsent(rbs[0], rbs[1]);
            }
        }, k, v);
    }

    @Override
    public V putIfAbsent(K k, V v, long lifespan, TimeUnit unit) {
        final long l = lifespan;
        final TimeUnit lu = unit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.putIfAbsent(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu));
            }
        }, k, v);
    }

    @Override
    public V putIfAbsent(K k, V v, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
        final long l = lifespan;
        final TimeUnit lu = lifespanTimeUnit;
        final long m = maxIdle;
        final TimeUnit mu = maxIdleUnit;
        return relayedInvoker(new RelayedMethod() {
            @Override
            public Object invoke(RelayBytes... rbs) {
                return jniRemoteCache.putIfAbsent(rbs[0], rbs[1], BigInteger.valueOf(l), timeunitToSwig(lu),
                        BigInteger.valueOf(m), timeunitToSwig(mu));
            }
        }, k, v);
    }

    @Override
    public void putAll(Map<? extends K, ? extends V> map) {
       putAll(map, 0, TimeUnit.MILLISECONDS, 0, TimeUnit.MILLISECONDS);
    }

    @Override
    public void putAll(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit) {
        putAll(map, lifespan, lifespanTimeUnit, 0, TimeUnit.MILLISECONDS);
    }

    @Override
    public void putAll(Map<? extends K, ? extends V> map, long lifespan, TimeUnit lifespanTimeUnit, long maxIdle, TimeUnit maxIdleUnit) {
       for (Entry<? extends K, ? extends V> entry : map.entrySet()) {
          put(entry.getKey(), entry.getValue(), lifespan, lifespanTimeUnit, maxIdle, maxIdleUnit);
       }
    }

    @Override
    public void clear() {
        if (jniRemoteCache != null) {
            jniRemoteCache.clear();
        }
    }

    @Override
    public boolean isEmpty() {
        return jniRemoteCache.isEmpty();
    }

    @Override
    public VersionedValue<V> getVersioned(K k) {
        if (jniRemoteCache == null) {
            return null;
        }

        final VersionPairReturn versionPair = relayedInvoker(new RelayedMethod() {
                @Override
                public Object invoke(RelayBytes... rbs) {
                    return jniRemoteCache.getWithVersion(rbs[0]);
                }
            }, k);

        if (versionPair.getFirst() == null || Hotrod.isNull(versionPair.getFirst())) {
            return null;
        }

        V value = relayedInvoker(new RelayedMethod() {
                @Override
                public Object invoke(RelayBytes... rbs) {
                    return Hotrod.dereference(versionPair.getFirst());
                }
            });

        if (value == null) {
            return null;
        }

        return new VersionedValueImpl(versionPair.getSecond().getVersion(), value);
    }

    @Override
    public MetadataValue<V> getWithMetadata(K k) {
        if (jniRemoteCache == null) {
            return null;
        }

        final MetadataPairReturn metadataPair = relayedInvoker(new RelayedMethod() {
               @Override
               public Object invoke(RelayBytes... rbs) {
                   return jniRemoteCache.getWithMetadata(rbs[0]);
               }
            }, k);

        if (metadataPair == null || metadataPair.getFirst() == null || Hotrod.isNull(metadataPair.getFirst())) {
            return null;
        }

       V value = relayedInvoker(new RelayedMethod() {
               @Override
               public Object invoke(RelayBytes... rbs) {
                   return Hotrod.dereference(metadataPair.getFirst());
               }
           });

        org.infinispan.client.hotrod.jni.MetadataValue metadata = metadataPair.getSecond();
        return new MetadataValueImpl(metadata.getCreated(), metadata.getLifespan(), metadata.getLastUsed(),
                metadata.getMaxIdle(), metadata.getVersion(), value);
    }

    @Override
    public Set<K> keySet() {
        final SetReturn setReturn = jniRemoteCache.keySet();
        final VectorReturn vectorReturn = Hotrod.keys(setReturn);
        Set<K> result = new HashSet<K>();
        for (int i = 0; i < vectorReturn.size(); i++) {
            final int index = i;
            K key = relayedInvoker(new RelayedMethod() {
                   @Override
                   public Object invoke(RelayBytes... rbs) {
                       return Hotrod.dereference(vectorReturn.get(index));
                   }
               });
            result.add(key);
        }

        return result;
    }

    @Override
    public Map<K, V> getBulk() {
        if (jniRemoteCache == null) {
            return null;
        }

        final MapReturn mapReturn = jniRemoteCache.getBulk();
        final VectorReturn vectorReturn = Hotrod.keySet(mapReturn);

        Map<K, V> result = new HashMap<K, V>();
        for (int i = 0; i < vectorReturn.size(); i++) {
            final int index = i;
            K key = relayedInvoker(new RelayedMethod() {
                    @Override
                    public Object invoke(RelayBytes... rbs) {
                        return Hotrod.dereference(vectorReturn.get(index));
                    }
                });
            V value = relayedInvoker(new RelayedMethod() {
                    @Override
                    public Object invoke(RelayBytes... rbs) {
                        return Hotrod.dereference(mapReturn.get(vectorReturn.get(index)));
                    }
                });

            result.put(key, value);
        }

        return result;
    }

    @Override
    public Map<K, V> getBulk(int size) {
        if (jniRemoteCache == null) {
            return null;
        }

        final MapReturn mapReturn = jniRemoteCache.getBulk(size);
        final VectorReturn vectorReturn = Hotrod.keySet(mapReturn);

        Map<K, V> result = new HashMap<K, V>();
        for (int i = 0; i < vectorReturn.size(); i++) {
            final int index = i;
            K key = relayedInvoker(new RelayedMethod() {
                    @Override
                    public Object invoke(RelayBytes... rbs) {
                        return Hotrod.dereference(vectorReturn.get(index));
                    }
                });
            V value = relayedInvoker(new RelayedMethod() {
                    @Override
                    public Object invoke(RelayBytes... rbs) {
                        return Hotrod.dereference(mapReturn.get(vectorReturn.get(index)));
                    }
                });

            result.put(key, value);
        }

        return result;
    }

    @Override
    public RemoteCache<K, V> withFlags(Flag... flags) {
        int result = 0;
        if (flags != null) {
            for (Flag flag : flags) {
                result |= flag.getFlagInt();
            }
        }
        jniRemoteCache.withFlags(org.infinispan.client.hotrod.jni.Flag.swigToEnum(result));
        return this;
    }

    private <R> R relayedInvoker(RelayedMethod m, Object... o) {
        RelayBytes rb[] = new RelayBytes[o.length];
        byte bytes[][] = new byte[o.length][];
        try {
            for (int i = 0; i < o.length; i++) {
                rb[i] = new RelayBytes();
                bytes[i] = marshaller.objectToByteBuffer(o[i]);
                setJvmBytes(rb[i], bytes[i]);
            }
            Object ret = m.invoke(rb);
            if (ret == null) {
                return null;
            } else if (ret instanceof RelayBytes) {
                RelayBytes retrb = (RelayBytes) ret;
                byte[] jcopy = new byte[(int) retrb.getLength()];
                readNative(retrb, jcopy);
                try {
                    return (R) marshaller.objectFromByteBuffer(jcopy);
                } finally {
                    dispose(retrb);
                }
            } else {
                return (R) ret;
            }
        } catch (RuntimeException e) {
            throw e;
        } catch (Throwable t) {
            throw new RuntimeException(t);
        } finally {
            for (int i = 0; i < o.length; i++) {
                releaseJvmBytes(rb[i], bytes[i]);
            }
        }
    }

    public interface RelayedMethod {
        Object invoke(RelayBytes... rbs);
    }

    @Override
    public int size() {
        return jniRemoteCache.size().intValue();
    }
}
