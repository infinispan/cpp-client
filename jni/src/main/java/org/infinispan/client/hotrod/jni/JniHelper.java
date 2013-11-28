// TODO: implement proper swig typemaps

package org.infinispan.client.hotrod.jni;

import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.HotrodJNI2;

public class JniHelper {
    public static void setJvmBytes(RelayBytes rb, byte[] ba) {
        HotrodJNI2.setJvmBytes(RelayBytes.getCPtr(rb), ba);
    }

    public static void releaseJvmBytes(RelayBytes rb, byte[] ba) {
        HotrodJNI2.releaseJvmBytes(RelayBytes.getCPtr(rb), ba);
    }

    public static void readNative(RelayBytes rb, byte[] ba) {
        HotrodJNI2.readNative(RelayBytes.getCPtr(rb), ba);
    }

    public static void dispose(RelayBytes rb) {
        if (rb == null)
            return;
        if (RelayBytes.getCPtr(rb) != 0) {
            rb.swigCMemOwn = true;
            rb.delete();
        }
    }

    public static org.infinispan.client.hotrod.jni.TimeUnit timeunitToSwig(java.util.concurrent.TimeUnit unit) {
        return org.infinispan.client.hotrod.jni.TimeUnit.swigToEnum(unit.ordinal());
     }
}
