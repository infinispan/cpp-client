// TODO: implement proper swig typemaps

package org.infinispan.client.hotrod.jni;

public class HotrodJNI2 {
    public final static native long new_JniTest();
    public final static native void setJvmBytes(long relayBytesCptr, byte[] ba);
    public final static native void releaseJvmBytes(long relayBytesCptr, byte[] ba);
    public final static native void readNative(long relayBytesCptr, byte[] ba);
}
