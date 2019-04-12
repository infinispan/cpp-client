package org.infinispan.client.hotrod.impl.protocol;

import java.util.HashMap;
import java.util.Map;

import static org.infinispan.client.hotrod.impl.ConfigurationProperties.*;

/**
 * Code factory.
 *
 * @author Galder Zamarreño
 * @since 5.1
 */
public class CodecFactory {
   private static final Map<String, Codec> codecMap;

   static {
      codecMap = new HashMap<String, Codec>();
   }

   public static Codec getCodec(String version) {
      if (codecMap.containsKey(version))
         return codecMap.get(version);
      else
         throw new IllegalArgumentException("Invalid Hot Rod protocol version");
   }

}
