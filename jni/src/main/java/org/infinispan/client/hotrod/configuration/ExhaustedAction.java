package org.infinispan.client.hotrod.configuration;

/**
 * Enumeration for whenExhaustedAction. Order is important, as the underlying commons-pool uses a byte to represent values
 * ExhaustedAction.
 *
 * @author Tristan Tarrant
 * @since 5.3
 */
public enum ExhaustedAction {
   EXCEPTION(org.infinispan.client.hotrod.jni.ExhaustedAction.EXCEPTION), // GenericKeyedObjectPool.WHEN_EXHAUSTED_FAIL
   WAIT(org.infinispan.client.hotrod.jni.ExhaustedAction.WAIT), // GenericKeyedObjectPool.WHEN_EXHAUSTED_BLOCK
   CREATE_NEW(org.infinispan.client.hotrod.jni.ExhaustedAction.CREATE_NEW); // GenericKeyedObjectPool.WHEN_EXHAUSTED_GROW

   private org.infinispan.client.hotrod.jni.ExhaustedAction jniExhaustedAction;
   
   ExhaustedAction(org.infinispan.client.hotrod.jni.ExhaustedAction exhaustedAction){
      this.jniExhaustedAction = exhaustedAction;
   }
   
   public int getExhaustedActionInt() {
      return jniExhaustedAction.swigValue();
   }
   
   public org.infinispan.client.hotrod.jni.ExhaustedAction getJniExhaustedAction() {
      return this.jniExhaustedAction;
   }
   
   public ExhaustedAction setExhaustedAction(org.infinispan.client.hotrod.jni.ExhaustedAction exhaustedAction){
      this.jniExhaustedAction = exhaustedAction;
      return this;
   }
}
