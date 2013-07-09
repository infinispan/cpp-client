// TODO: implement proper swig typemaps

package org.infinispan.client.hotrod.jni;

import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.HotrodJNI2;

public class JniHelper {
    /*
    public static void testit() {
	// ZZZ
	RelayBytes rbytes = new RelayBytes();
	byte[] tbytes = new byte[4];
	tbytes[0] = (byte) 'a';
	tbytes[1] = (byte) 'c';
	tbytes[2] = (byte) 'x';
	tbytes[3] = 0;
	HotrodJNI2.setJvmBytes(RelayBytes.getCPtr(rbytes), tbytes);
	HotrodJNI2.releaseJvmBytes(RelayBytes.getCPtr(rbytes), tbytes);
    }
    */

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
}

