package org.infinispan.client.jni.hotrod;

import static org.infinispan.client.hotrod.test.HotRodClientTestingUtil.startHotRodServer;
import static org.infinispan.server.hotrod.test.HotRodTestingUtil.hotRodCacheConfiguration;
import static org.infinispan.test.TestingUtil.killCacheManagers;
import static org.infinispan.client.hotrod.test.HotRodClientTestingUtil.killServers;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.fail;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.math.BigInteger;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

import org.infinispan.client.hotrod.jni.Flag;
import org.infinispan.client.hotrod.jni.Hotrod;
import org.infinispan.client.hotrod.jni.JniHelper;
import org.infinispan.client.hotrod.jni.MapReturn;
import org.infinispan.client.hotrod.jni.MetadataPairReturn;
import org.infinispan.client.hotrod.jni.RelayBytes;
import org.infinispan.client.hotrod.jni.RemoteCache_jb_jb;
import org.infinispan.client.hotrod.jni.VectorReturn;
import org.infinispan.client.hotrod.jni.VersionPairReturn;
import org.infinispan.client.hotrod.logging.Log;
import org.infinispan.client.hotrod.logging.LogFactory;
import org.infinispan.commons.marshall.Marshaller;
import org.infinispan.configuration.cache.ConfigurationBuilder;
import org.infinispan.manager.EmbeddedCacheManager;
import org.infinispan.server.hotrod.HotRodServer;
import org.infinispan.test.SingleCacheManagerTest;
import org.infinispan.test.fwk.TestCacheManagerFactory;
import org.testng.annotations.AfterClass;
import org.testng.annotations.AfterMethod;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

/**
 * Tests C++ and Java Hot Rod Client interoperability.
 * 
 * @author Alan Field
 */
@Test(testName = "client.hotrod.CrossLanguageHotRodTest", groups = "functional")
public class CrossLanguageHotRodTest extends SingleCacheManagerTest {
   final String DEFAULT_CACHE_MANAGER = "local";
   final String DEFAULT_CACHE = "testcache";

   private static final Log log = LogFactory.getLog(CrossLanguageHotRodTest.class);

   Object javaRemoteCacheManagerObject;
   Object javaCacheObject;
   org.infinispan.client.hotrod.jni.RemoteCacheManager cppRemoteCacheManager;
   RemoteCache_jb_jb cppCache;
   Marshaller marshaller = new org.infinispan.commons.marshall.jboss.GenericJBossMarshaller();

   //Test data
   String v01 = "v0";
   String v02 = "ÅÄÇÉÑÖÕÜàäâáãçëèêéîïìíñôöòóüûùúÿ";
   String v03 = null;

   byte[] v11 = { 'v', 'a', 'l', 'u', 'e', '1' };
   boolean[] v12 = { true, false, false, true };
   char[] v13 = { 'v', 'à', 'l', 'û', 'è', '1' };
   double[] v14 = { Double.NEGATIVE_INFINITY, Double.POSITIVE_INFINITY, Double.MAX_VALUE, Double.MIN_VALUE,
         Double.MIN_NORMAL, Double.NaN, 0 };
   float[] v15 = { Float.NEGATIVE_INFINITY, Float.POSITIVE_INFINITY, Float.MAX_VALUE, Float.MIN_VALUE,
         Float.MIN_NORMAL, Float.NaN, 0 };
   int[] v16 = { Integer.MAX_VALUE, Integer.MIN_VALUE, 0 };
   long[] v17 = { Long.MAX_VALUE, Long.MIN_VALUE, 0 };
   short[] v18 = { Short.MAX_VALUE, Short.MIN_VALUE, 0 };
   String[] v19 = { "ÅÄ", "Ç", "É", "Ñ", "ÖÕ", "Ü", "àäâáã", "ç", "ëèêé", "îïìí", "ñ", "ôöòó", "üûùú", "ÿ", null };

   boolean v21 = true;
   boolean v22 = false;

   byte v31 = 127;
   byte v32 = -128;
   byte v33 = 0;

   char v41 = '4';
   char v42 = 'Ç';

   double v51 = Double.NEGATIVE_INFINITY;
   double v52 = Double.POSITIVE_INFINITY;
   double v53 = Double.MAX_VALUE;
   double v54 = Double.MIN_VALUE;
   double v55 = Double.MIN_NORMAL;
   double v56 = Double.NaN;
   double v57 = 0;

   float v61 = Float.NEGATIVE_INFINITY;
   float v62 = Float.POSITIVE_INFINITY;
   float v63 = Float.MAX_VALUE;
   float v64 = Float.MIN_VALUE;
   float v65 = Float.MIN_NORMAL;
   float v66 = Float.NaN;
   float v67 = 0;

   int v71 = Integer.MIN_VALUE;
   int v72 = Integer.MAX_VALUE;
   int v73 = 0;

   long v81 = Long.MAX_VALUE;
   long v82 = Long.MIN_VALUE;
   long v83 = 0;

   short v91 = Short.MIN_VALUE;
   short v92 = Short.MAX_VALUE;
   short v93 = 0;

   Object v10 = null;

   Object[] valueArray = { v01, v02, v03, v11, v12, v13, v14, v15, v16, v17, v18, v19, v21, v22, v31, v32, v33, v41,
         v42, v51, v52, v53, v54, v55, v56, v57, v61, v62, v63, v64, v65, v66, v67, v71, v72, v73, v81, v82, v83, v91,
         v92, v93, v10 };

   String serverConfigPath = System.getProperty("server1.dist") + File.separator + "standalone" + File.separator
         + "configuration";

   String[] lifespanMaxIdleCommands = { "put", "putIfAbsent", "replace", "replaceWithVersion" };

   private URLClassLoader ucl;
   private HotRodServer hotrodServer;
   private Method javaGetCacheMethod;
   private Method javaPutMethod;
   private Method javaGetMethod;
   private Method javaContainsKeyMethod;
   private Method javaGetWithMetadataMethod;
   private Method javaGetCreatedMethod;
   private Method javaGetLastUsedMethod;
   private Method javaGetLifespanMethod;
   private Method javaGetMaxIdleMethod;
   private Method javaGetVersionMethod;
   private Method javaStatsMethod;
   private Method javaGetStatsMapMethod;
   private Method javaAddServersMethod;
   private Method javaConfBuildMethod;

   @Override
   protected EmbeddedCacheManager createCacheManager() throws Exception {
      // Enable statistics in the global configuration
      Object config = hotRodCacheConfiguration();
      ((ConfigurationBuilder) config).jmxStatistics().enable();

      cacheManager = TestCacheManagerFactory.createCacheManager((ConfigurationBuilder) config);
      cacheManager.defineConfiguration(DEFAULT_CACHE, ((ConfigurationBuilder) config).build());

      hotrodServer = startHotRodServer(cacheManager);
      org.infinispan.client.hotrod.jni.ConfigurationBuilder cppBuilder = new org.infinispan.client.hotrod.jni.ConfigurationBuilder();
      cppBuilder.addServer().host("localhost").port(hotrodServer.getPort());

      cppRemoteCacheManager = new org.infinispan.client.hotrod.jni.RemoteCacheManager(cppBuilder.build(), false);

      assertFalse(cppRemoteCacheManager.isStarted());
      cppRemoteCacheManager.start();
      assertTrue(cppRemoteCacheManager.isStarted());
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      getJavaHotRodClasses();

      return cacheManager;
   }

   private void getJavaHotRodClasses() throws Exception {
      /*
       * Create a new ClassLoader object that does not reference the JNI Wrapper classes with the
       * same name as the classes in the Java Hot Rod client JAR file
       */
      String[] classpath = System.getProperty("java.class.path").split(File.pathSeparator);
      ArrayList<URL> jars = new ArrayList<URL>();
      for (String path : classpath) {
         if (path.matches(".*infinispan-client-hotrod.*\\.jar")) {
            jars.add(new File(path).toURI().toURL());
         }
      }
      ucl = new URLClassLoader(jars.toArray(new URL[0]), null) {
              protected Class<?> findClass(String name) throws ClassNotFoundException {
                  try {
                      /* Try to find the class in the java hotrod client jar first. */
                      return super.findClass(name);
                  } catch (ClassNotFoundException ex) {
                      /* Delegate to the main CL if it's not a class from that jar. */
                      return getClass().getClassLoader().loadClass(name);
                  }
              }
          };

      Class<?> javaRCMClass = ucl.loadClass("org.infinispan.client.hotrod.RemoteCacheManager");
      Class javaConfClass = ucl.loadClass("org.infinispan.client.hotrod.configuration.Configuration");
      Class javaConfBuildClass = ucl.loadClass("org.infinispan.client.hotrod.configuration.ConfigurationBuilder");
      try {
         javaRCMClass.getMethod("getJniManager");
         fail("Could not load Java Hot Rod Client RemoteCacheManager");
      } catch (Exception e) {
         // This should throw an exception
      }
      Constructor<?> ctor = javaRCMClass.getConstructor(new Class[] { javaConfClass });
      javaGetCacheMethod = javaRCMClass.getDeclaredMethod("getCache", new Class[] { String.class });
      javaAddServersMethod= javaConfBuildClass.getDeclaredMethod("addServers", new Class[] { String.class });
      javaConfBuildMethod = javaConfBuildClass.getDeclaredMethod("build", null);
      Object newInstance = javaConfBuildClass.getConstructor(null).newInstance();
      Object invoke = javaAddServersMethod.invoke(newInstance, "localhost:"+hotrodServer.getPort());
      javaRemoteCacheManagerObject = ctor.newInstance(javaConfBuildMethod.invoke(invoke));

      javaCacheObject = javaGetCacheMethod.invoke(javaRemoteCacheManagerObject, DEFAULT_CACHE);
      Class<?> javaRCClass = ucl.loadClass("org.infinispan.client.hotrod.RemoteCache");

      javaPutMethod = javaRCClass.getMethod("put", new Class[] { Object.class, Object.class });
      javaGetMethod = javaRCClass.getMethod("get", new Class[] { Object.class });
      javaContainsKeyMethod = javaRCClass.getMethod("containsKey", new Class[] { Object.class });
      javaGetWithMetadataMethod = javaRCClass.getMethod("getWithMetadata", new Class[] { Object.class });
      javaStatsMethod = javaRCClass.getMethod("stats", new Class[] {});

      Class<?> javaSSClass = ucl.loadClass("org.infinispan.client.hotrod.ServerStatistics");
      javaGetStatsMapMethod = javaSSClass.getMethod("getStatsMap", new Class[] {});

      Class<?> javaMVClass = ucl.loadClass("org.infinispan.client.hotrod.MetadataValue");
      javaGetCreatedMethod = javaMVClass.getMethod("getCreated");
      javaGetLastUsedMethod = javaMVClass.getMethod("getLastUsed");
      javaGetLifespanMethod = javaMVClass.getMethod("getLifespan");
      javaGetMaxIdleMethod = javaMVClass.getMethod("getMaxIdle");
      javaGetVersionMethod = javaMVClass.getMethod("getVersion");
   }

   /*
    * test methods
    */

   @Test
   public void testCppPut() throws Exception {
      log.info("doCppPut()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         assertEquals(null, cppPut(cppCache, "k" + i, valueArray[i], true, -1, -1));
         assertEquals(i + 1, cppCache.size().longValue());
      }

      assertEquals(valueArray.length, cppCache.size().longValue());

      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(checkEquality(valueArray[i], javaGetMethod.invoke(javaCacheObject, "k" + i)), "Expected: ("
               + valueArray[i] + "), Actual: (" + javaGetMethod.invoke(javaCacheObject, "k" + i) + ")");
      }

      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppReplace() throws Exception {
      log.info("doCppReplace()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }

      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         if (i % 2 == 0) {
            Object replaceResult = cppReplace(cppCache, "k" + i, "v" + (i * 10));
            assertTrue(checkEquality(valueArray[i], replaceResult), "Expected: (" + valueArray[i] + "), Actual: ("
                  + replaceResult + ")");
         } else {
            assertEquals(null, cppReplace(cppCache, "k" + i, "v" + (i * 10), false, -1, -1));
         }
      }
      for (int i = 0; i < valueArray.length; i++) {
         assertEquals("v" + (i * 10), javaGetMethod.invoke(javaCacheObject, "k" + i));
      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppGet() throws Exception {
      log.info("doCppGet()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(checkEquality(valueArray[i], cppGet(cppCache, "k" + i)), "Expected: (" + valueArray[i]
               + "), Actual: (" + cppGet(cppCache, "k" + i) + ")");
      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppGetBulk() throws Exception {
      log.info("doCppGetBulk()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());

      /*
       * getBulk(valueArray.length)
       */
      Map<Object, Object> result = cppGetBulk(cppCache, valueArray.length);
      assertEquals(valueArray.length, result.size());
      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(checkEquality(valueArray[i], result.get("k" + i)), "Expected: (" + valueArray[i] + "), Actual: ("
               + result.get("k" + i) + ")");
      }

      /*
       * getBulk(0)
       */
      result = cppGetBulk(cppCache, 0);
      assertEquals(valueArray.length, result.size());
      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(checkEquality(valueArray[i], result.get("k" + i)), "Expected: (" + valueArray[i] + "), Actual: ("
               + result.get("k" + i) + ")");
      }

      /*
       * getBulk(valueArray.length - 1)
       */
      result = cppGetBulk(cppCache, valueArray.length - 1);
      assertEquals(valueArray.length - 1, result.size());
      for (int i = 0; i < valueArray.length; i++) {
         if (result.containsKey("k" + i)) {
            assertTrue(checkEquality(valueArray[i], result.get("k" + i)), "Expected: (" + valueArray[i]
                  + "), Actual: (" + result.get("k" + i) + ")");
         }
      }

      /*
       * getBulk(1)
       */
      result = cppGetBulk(cppCache, 1);
      assertEquals(1, result.size());
      for (int i = 0; i < valueArray.length; i++) {
         if (result.containsKey("k" + i)) {
            assertTrue(checkEquality(valueArray[i], result.get("k" + i)), "Expected: (" + valueArray[i]
                  + "), Actual: (" + result.get("k" + i) + ")");
         }
      }

      /*
       * getBulk(valueArray.length + 1)
       */
      result = cppGetBulk(cppCache, valueArray.length + 1);
      assertEquals(valueArray.length, result.size());
      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(checkEquality(valueArray[i], result.get("k" + i)), "Expected: (" + valueArray[i] + "), Actual: ("
               + result.get("k" + i) + ")");
      }

      /*
       * getBulk(-1)
       */
      result = cppGetBulk(cppCache, -1);
      assertEquals(0, result.size());

      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppRemove() throws Exception {
      log.info("doCppRemove()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         if (i % 2 == 0) {
            Object removeResult = cppRemove(cppCache, "k" + i, true);
            assertTrue(checkEquality(valueArray[i], removeResult), "Expected: (" + valueArray[i] + "), Actual: ("
                  + removeResult + ")");
         } else {
            assertEquals(null, cppRemove(cppCache, "k" + i, false));
         }
         assertEquals(valueArray.length - (i + 1), cppCache.size().longValue());
      }
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppContainsKey() throws Exception {
      log.info("doCppContainsKey()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         assertEquals(true, cppContainsKey(cppCache, "k" + i));
      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppPutIfAbsent() throws Exception {
      log.info("doCppPutIfAbsent()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         assertFalse(cppContainsKey(cppCache, "k" + i));
         assertEquals(null, cppPutIfAbsent(cppCache, "k" + i, valueArray[i], true, -1, -1));
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         assertTrue(cppContainsKey(cppCache, "k" + i));
         assertTrue(checkEquality(valueArray[i], javaGetMethod.invoke(javaCacheObject, "k" + i)), "Expected: ("
               + valueArray[i] + "), Actual: (" + javaGetMethod.invoke(javaCacheObject, "k" + i) + ")");
         assertTrue(checkEquality(valueArray[i], cppGet(cppCache, "k" + i)), "Expected: (" + valueArray[i]
               + "), Actual: (" + cppGet(cppCache, "k" + i) + ")");

         if (i % 2 == 0) {
            assertTrue(checkEquality(valueArray[i], cppPutIfAbsent(cppCache, "k" + i, "newValue", true, -1, -1)));
         } else {
            assertEquals(null, cppPutIfAbsent(cppCache, "k" + i, "newValue", false, -1, -1));
         }
         assertTrue(checkEquality(valueArray[i], javaGetMethod.invoke(javaCacheObject, "k" + i)), "Expected: ("
               + valueArray[i] + "), Actual: (" + javaGetMethod.invoke(javaCacheObject, "k" + i) + ")");
         assertTrue(checkEquality(valueArray[i], cppGet(cppCache, "k" + i)), "Expected: (" + valueArray[i]
               + "), Actual: (" + cppGet(cppCache, "k" + i) + ")");
      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppLifespan() throws Exception {
      log.info("doCppLifespan()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);
      long lifespanSec = 2;

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (String command : lifespanMaxIdleCommands) {
         if (command.equals(lifespanMaxIdleCommands[0])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPut(cppCache, "k" + i, valueArray[i], false, lifespanSec, -1));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
            }
         } else if (command.equals(lifespanMaxIdleCommands[1])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPutIfAbsent(cppCache, "k" + i, valueArray[i], false, lifespanSec, -1));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
            }
         } else if (command.equals(lifespanMaxIdleCommands[2])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
               assertEquals(null, cppReplace(cppCache, "k" + i, valueArray[i], false, lifespanSec, -1));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
            }
         } else if (command.equals(lifespanMaxIdleCommands[3])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, "javaData");
               long currentVersion = cppGetWithVersion(cppCache, "k" + i).getSecond().getVersion();

               assertTrue(cppReplaceWithVersion(cppCache, "k" + i, currentVersion, valueArray[i], lifespanSec, -1));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
            }
         }

         cppClear(cppCache, DEFAULT_CACHE);
         assertTrue(cppCache.isEmpty());
         assertEquals(0, cppCache.size().longValue());
      }
   }

   @Test
   public void testCppMaxIdle() throws Exception {
      log.info("doCppMaxIdle()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);
      long maxIdleSec = 2;

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (String command : lifespanMaxIdleCommands) {
         if (command.equals(lifespanMaxIdleCommands[0])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPut(cppCache, "k" + i, valueArray[i], false, -1, maxIdleSec));
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());

               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[1])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPutIfAbsent(cppCache, "k" + i, valueArray[i], false, -1, maxIdleSec));
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[2])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
               assertEquals(null, cppReplace(cppCache, "k" + i, valueArray[i], false, -1, maxIdleSec));
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[3])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, "javaData");
               long currentVersion = cppGetWithVersion(cppCache, "k" + i).getSecond().getVersion();

               assertTrue(cppReplaceWithVersion(cppCache, "k" + i, currentVersion, valueArray[i], -1, maxIdleSec));
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         }

         cppClear(cppCache, DEFAULT_CACHE);
         assertTrue(cppCache.isEmpty());
         assertEquals(0, cppCache.size().longValue());
      }
   }

   @Test
   public void testCppLifespanAndMaxIdle() throws Exception {
      log.info("doCppLifespanAndMaxIdle()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);
      long lifespanSec = 2;
      long maxIdleSec = lifespanSec / 2;

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());

      for (String command : lifespanMaxIdleCommands) {
         if (command.equals(lifespanMaxIdleCommands[0])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPut(cppCache, "k" + i, valueArray[i], false, lifespanSec, maxIdleSec));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[1])) {
            for (int i = 0; i < valueArray.length; i++) {
               assertEquals(null, cppPutIfAbsent(cppCache, "k" + i, valueArray[i], false, lifespanSec, maxIdleSec));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[2])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, "javaData");
               assertEquals(null, cppReplace(cppCache, "k" + i, valueArray[i], false, lifespanSec, maxIdleSec));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         } else if (command.equals(lifespanMaxIdleCommands[3])) {
            for (int i = 0; i < valueArray.length; i++) {
               javaPutMethod.invoke(javaCacheObject, "k" + i, "javaData");
               long currentVersion = cppGetWithVersion(cppCache, "k" + i).getSecond().getVersion();

               assertTrue(cppReplaceWithVersion(cppCache, "k" + i, currentVersion, valueArray[i], lifespanSec,
                     maxIdleSec));
               assertEquals(lifespanSec, ((Integer) javaGetLifespanMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(lifespanSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getLifespan());
               assertEquals(maxIdleSec, ((Integer) javaGetMaxIdleMethod.invoke(javaGetWithMetadataMethod.invoke(
                     javaCacheObject, "k" + i))).intValue());
               assertEquals(maxIdleSec, this.cppGetWithMetadata(cppCache, "k" + i).getSecond().getMaxIdle());
            }
         }

         cppClear(cppCache, DEFAULT_CACHE);
         assertTrue(cppCache.isEmpty());
         assertEquals(0, cppCache.size().longValue());
      }
   }

   @Test
   public void testCppStats() throws Exception {
      log.info("doCppStats()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());

      Object stats = javaStatsMethod.invoke(javaCacheObject);
      @SuppressWarnings("unchecked")
      Map<String, String> statsMap = (Map<String, String>) javaGetStatsMapMethod.invoke(stats);

      for (String key : statsMap.keySet()) {
         assertTrue(cppCache.stats().has_key(key));
         log.info("Key: " + key + "; Value: " + cppGetLongCacheStat(cppCache, key));
      }

      assertEquals(0, cppGetLongCacheStat(cppCache, "currentNumberOfEntries"));

      long totalNumberOfEntries = cppGetLongCacheStat(cppCache, "totalNumberOfEntries");
      long stores = cppGetLongCacheStat(cppCache, "stores");

      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppGetLongCacheStat(cppCache, "currentNumberOfEntries"));
         assertEquals(totalNumberOfEntries + i + 1, cppGetLongCacheStat(cppCache, "totalNumberOfEntries"));
         assertEquals(stores + i + 1, cppGetLongCacheStat(cppCache, "stores"));
      }

      //Store initial values
      assertEquals(cppCache.size().longValue(), cppGetLongCacheStat(cppCache, "currentNumberOfEntries"));
      long hits = cppGetLongCacheStat(cppCache, "hits");
      long removeMisses = cppGetLongCacheStat(cppCache, "removeMisses");
      long removeHits = cppGetLongCacheStat(cppCache, "removeHits");
      long retrievals = cppGetLongCacheStat(cppCache, "retrievals");
      long misses = cppGetLongCacheStat(cppCache, "misses");

      //hit
      assertTrue(checkEquality(valueArray[0], cppGet(cppCache, "k0")));
      assertEquals(hits + 1, cppGetLongCacheStat(cppCache, "hits"));
      assertEquals(retrievals + 1, cppGetLongCacheStat(cppCache, "retrievals"));
      //miss
      assertTrue(checkEquality(null, cppGet(cppCache, "NON_EXISTENT")));
      assertEquals(misses + 1, cppGetLongCacheStat(cppCache, "misses"));
      assertEquals(retrievals + 2, cppGetLongCacheStat(cppCache, "retrievals"));
      //removeHits
      assertEquals(null, cppRemove(cppCache, "k0", false));
      assertEquals(removeHits + 1, cppGetLongCacheStat(cppCache, "removeHits"));
      assertEquals(cppCache.size().longValue(), cppGetLongCacheStat(cppCache, "currentNumberOfEntries"));
      //removeMisses
      assertEquals(null, cppRemove(cppCache, "NON_EXISTENT", false));
      assertEquals(removeMisses + 1, cppGetLongCacheStat(cppCache, "removeMisses"));

      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppReplaceWithVersion() throws Exception {
      log.info("doCppReplaceWithVersion()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }

      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         long currentVersion = cppGetWithVersion(cppCache, "k" + i).getSecond().getVersion();

         assertFalse(cppReplaceWithVersion(cppCache, "k" + i, currentVersion + 1, "replacedValue", -1, -1));
         assertTrue(checkEquality(valueArray[i], javaGetMethod.invoke(javaCacheObject, "k" + i)), "Expected: ("
               + valueArray[i] + "), Actual: (" + javaGetMethod.invoke(javaCacheObject, "k" + i) + ")");
         assertTrue(cppReplaceWithVersion(cppCache, "k" + i, currentVersion, "replacedValue", -1, -1));
         assertTrue(checkEquality("replacedValue", javaGetMethod.invoke(javaCacheObject, "k" + i)),
               "Expected: (replacedValue), Actual: (" + javaGetMethod.invoke(javaCacheObject, "k" + i) + ")");
      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppRemoveWithVersion() throws Exception {
      log.info("doCppRemoveWithVersion()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         long currentVersion = cppGetWithVersion(cppCache, "k" + i).getSecond().getVersion();

         assertFalse(cppRemoveWithVersion(cppCache, "k" + i, currentVersion + 1));
         assertTrue(cppContainsKey(cppCache, "k" + i));
         assertTrue(cppRemoveWithVersion(cppCache, "k" + i, currentVersion));
         assertFalse(cppContainsKey(cppCache, "k" + i));
         assertEquals(valueArray.length - (i + 1), cppCache.size().longValue());
      }
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   @Test
   public void testCppGetWithMetadata() throws Exception {
      log.info("doCppGetWithMetadata()");
      cppCache = Hotrod.getJniRelayNamedCache(cppRemoteCacheManager, DEFAULT_CACHE, false);

      assertTrue(cppRemoteCacheManager.isStarted());
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         javaPutMethod.invoke(javaCacheObject, "k" + i, valueArray[i]);
         assertEquals(i + 1, cppCache.size().longValue());
      }
      assertEquals(valueArray.length, cppCache.size().longValue());
      for (int i = 0; i < valueArray.length; i++) {
         MetadataPairReturn result = cppGetWithMetadata(cppCache, "k" + i);
         Object javaMeta = javaGetWithMetadataMethod.invoke(javaCacheObject, "k" + i);
         // From Java: MetadataValueImpl [created=-1, lifespan=-1, lastUsed=-1, maxIdle=-1, getVersion()=1334, getValue()=v0]

         if (result.getFirst() != null) {
            RelayBytes vrb = null;
            try {
               vrb = Hotrod.dereference(result.getFirst());
               if (vrb != null) {
                  Object value = unmarshall(vrb);
                  org.infinispan.client.hotrod.jni.MetadataValue cppMeta = result.getSecond();
                  assertTrue(checkEquality(valueArray[i], value), "Expected: (" + valueArray[i] + "), Actual: ("
                        + value + ")");
                  assertEquals(javaGetCreatedMethod.invoke(javaMeta), cppMeta.getCreated());
                  assertEquals(javaGetLastUsedMethod.invoke(javaMeta), cppMeta.getLastUsed());
                  assertEquals(javaGetLifespanMethod.invoke(javaMeta), cppMeta.getLifespan());
                  assertEquals(javaGetMaxIdleMethod.invoke(javaMeta), cppMeta.getMaxIdle());
                  assertEquals(javaGetVersionMethod.invoke(javaMeta), cppMeta.getVersion());
               }
            } finally {
               JniHelper.dispose(vrb);
            }
         }

      }
      cppClear(cppCache, DEFAULT_CACHE);
      assertTrue(cppCache.isEmpty());
      assertEquals(0, cppCache.size().longValue());
   }

   /*
    * RemoteCache_jb_jb helper methods
    */
   private long cppGetLongCacheStat(RemoteCache_jb_jb cppCache, String statName) {
      return Long.parseLong(cppCache.stats().get(statName));
   }

   private Object cppPut(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue, long lifespan,
         long maxIdle) {
      log.debug("cppPut(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue)");
      Object result = null;
      RelayBytes krb = marshall(key);
      RelayBytes vrb = marshall(value);
      try {
         if (forceReturnValue) {
            cache = cache.withFlags(Flag.FORCE_RETURN_VALUE);
         }
         if (maxIdle > 0) {
            result = unmarshall(cache.put(krb, vrb, BigInteger.valueOf(lifespan), BigInteger.valueOf(maxIdle)));
         } else {
            if (lifespan > 0) {
               result = unmarshall(cache.put(krb, vrb, BigInteger.valueOf(lifespan)));
            } else {
               result = unmarshall(cache.put(krb, vrb));
            }
         }
      } finally {
         JniHelper.dispose(krb);
         JniHelper.dispose(vrb);
      }
      return result;
   }

   private Object cppGet(RemoteCache_jb_jb cache, Object key) {
      log.debug("cppGet(RemoteCache_jb_jb cache, Object key)");
      RelayBytes krb = marshall(key);
      Object result = null;
      try {
         result = unmarshall(cache.get(krb));
      } finally {
         JniHelper.dispose(krb);
      }
      return result;
   }

   private Map<Object, Object> cppGetBulk(RemoteCache_jb_jb cache, int nrOfEntries) {
      MapReturn mapReturn = cache.getBulk(nrOfEntries);
      VectorReturn vectorReturn = Hotrod.keySet(mapReturn);

      Map<Object, Object> result = new HashMap<Object, Object>();
      RelayBytes krb = null;
      RelayBytes vrb = null;
      for (int i = 0; i < vectorReturn.size(); i++) {
         try {
            krb = Hotrod.dereference(vectorReturn.get(i));
            vrb = Hotrod.dereference(mapReturn.get(vectorReturn.get(i)));
            result.put(unmarshall(krb), unmarshall(vrb));
         } finally {
            JniHelper.dispose(krb);
            JniHelper.dispose(vrb);
         }
      }

      return result;
   }

   private MetadataPairReturn cppGetWithMetadata(RemoteCache_jb_jb cache, Object key) {
      RelayBytes krb = marshall(key);
      MetadataPairReturn result = null;
      try {
         result = cache.getWithMetadata(krb);
      } finally {
         JniHelper.dispose(krb);
      }
      return result;
   }

   private VersionPairReturn cppGetWithVersion(RemoteCache_jb_jb cache, Object key) {
      RelayBytes krb = marshall(key);
      VersionPairReturn result = null;
      try {
         result = cache.getWithVersion(krb);
      } finally {
         JniHelper.dispose(krb);
      }
      return result;
   }

   private Object cppRemove(RemoteCache_jb_jb cache, Object key, boolean forceReturnValue) {
      log.debug("cppRemove(RemoteCache_jb_jb cache, Object key, boolean forceReturnValue)");
      RelayBytes krb = marshall(key);
      Object result = null;
      try {
         if (forceReturnValue) {
            result = unmarshall(cache.withFlags(Flag.FORCE_RETURN_VALUE).remove(krb));
         } else {
            result = unmarshall(cache.remove(krb));
         }
      } finally {
         JniHelper.dispose(krb);
      }
      return result;
   }

   private boolean cppRemoveWithVersion(RemoteCache_jb_jb cache, Object key, long version) {
      log.debug("cppRemoveWithVersion(RemoteCache_jb_jb cache, Object key, long version)");
      RelayBytes krb = marshall(key);
      boolean result = false;
      try {
         result = cache.removeWithVersion(krb, BigInteger.valueOf(version));
      } finally {
         JniHelper.dispose(krb);
      }
      return result;
   }

   private boolean cppContainsKey(RemoteCache_jb_jb cache, Object key) {
      log.debug("cppContainsKey(RemoteCache_jb_jb cache, Object key");
      RelayBytes krb = marshall(key);
      try {
         return cache.containsKey(krb);
      } finally {
         JniHelper.dispose(krb);
      }
   }

   private Object cppReplace(RemoteCache_jb_jb cache, Object key, Object value) {
      log.debug("cppReplace(RemoteCache_jb_jb cache, Object key, Object value)");
      return cppReplace(cache, key, value, true, -1, -1);
   }

   private Object cppReplace(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue,
         long lifespan, long maxIdle) {
      log.debug("cppReplace(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue)");
      Object result = null;
      RelayBytes krb = marshall(key);
      RelayBytes vrb = marshall(value);
      try {
         if (forceReturnValue) {
            cache = cache.withFlags(Flag.FORCE_RETURN_VALUE);
         }
         if (maxIdle > 0) {
            result = unmarshall(cache.replace(krb, vrb, BigInteger.valueOf(lifespan), BigInteger.valueOf(maxIdle)));
         } else {
            if (lifespan > 0) {
               result = unmarshall(cache.replace(krb, vrb, BigInteger.valueOf(lifespan)));
            } else {
               result = unmarshall(cache.replace(krb, vrb));
            }
         }
      } finally {
         JniHelper.dispose(krb);
         JniHelper.dispose(vrb);
      }
      return result;
   }

   private boolean cppReplaceWithVersion(RemoteCache_jb_jb cache, Object key, long version, Object value,
         long lifespan, long maxIdle) {
      log.debug("cppReplaceWithVersion(RemoteCache_jb_jb cache, Object key, long version, Object value, long lifespan, long maxIdle)");
      boolean result = false;
      RelayBytes krb = marshall(key);
      RelayBytes vrb = marshall(value);
      try {
         if (maxIdle > 0) {
            result = cache.replaceWithVersion(krb, vrb, BigInteger.valueOf(version), BigInteger.valueOf(lifespan),
                  BigInteger.valueOf(maxIdle));
         } else {
            if (lifespan > 0) {
               result = cache.replaceWithVersion(krb, vrb, BigInteger.valueOf(version), BigInteger.valueOf(lifespan));
            } else {
               result = cache.replaceWithVersion(krb, vrb, BigInteger.valueOf(version));
            }
         }
      } finally {
         JniHelper.dispose(krb);
         JniHelper.dispose(vrb);
      }
      return result;
   }

   private Object cppPutIfAbsent(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue,
         long lifespan, long maxIdle) {
      log.debug("cppPutIfAbsent(RemoteCache_jb_jb cache, Object key, Object value, boolean forceReturnValue)");
      Object result = null;
      RelayBytes krb = marshall(key);
      RelayBytes vrb = marshall(value);
      try {
         if (forceReturnValue) {
            cache = cache.withFlags(Flag.FORCE_RETURN_VALUE);
         }
         if (maxIdle > 0) {
            result = unmarshall(cache.putIfAbsent(krb, vrb, BigInteger.valueOf(lifespan), BigInteger.valueOf(maxIdle)));
         } else {
            if (lifespan > 0) {
               result = unmarshall(cache.putIfAbsent(krb, vrb, BigInteger.valueOf(lifespan)));
            } else {
               result = unmarshall(cache.putIfAbsent(krb, vrb));
            }
         }
      } finally {
         JniHelper.dispose(krb);
         JniHelper.dispose(vrb);
      }
      return result;
   }

   private void cppClear(RemoteCache_jb_jb cache, String cacheName) {
      log.debug("cppClear()");
      cache.clear();
   }

   /*
    * Marshalling utilities
    */

   private RelayBytes marshall(Object source) {
      log.debug("marshall(Object source)");
      RelayBytes result = new RelayBytes();
      try {
         byte[] sourceBytes = marshaller.objectToByteBuffer(source);
         JniHelper.setJvmBytes(result, sourceBytes);
      } catch (Exception e) {
         log.error("Marshall error");
         e.printStackTrace();
      }
      return result;
   }

   private Object unmarshall(RelayBytes nativeSource) {
      log.debug("unmarshall(RelayBytes nativeSource)");
      Object result = null;
      if (nativeSource != null) {
         byte[] jcopy = new byte[(int) nativeSource.getLength()];
         JniHelper.readNative(nativeSource, jcopy);
         try {
            result = marshaller.objectFromByteBuffer(jcopy);
         } catch (Exception e) {
            log.error("Unmarshall error");
            e.printStackTrace();
         }
      }
      JniHelper.dispose(nativeSource);
      return result;
   }

   /**
    * 
    * Utility method to test object equality including arrays
    * 
    * @param obj1
    *           first object
    * @param obj2
    *           second object
    * @return <code>true</code> if the objects are equal, else <code>false</code>
    */
   private boolean checkEquality(Object obj1, Object obj2) {
      boolean result = false;
      if (obj1 != null && obj1.getClass().isArray()) {
         if (obj1 instanceof byte[]) {
            result = Arrays.equals((byte[]) obj1, (byte[]) obj2);
         } else if (obj1 instanceof boolean[]) {
            result = Arrays.equals((boolean[]) obj1, (boolean[]) obj2);
         } else if (obj1 instanceof char[]) {
            result = Arrays.equals((char[]) obj1, (char[]) obj2);
         } else if (obj1 instanceof double[]) {
            result = Arrays.equals((double[]) obj1, (double[]) obj2);
         } else if (obj1 instanceof float[]) {
            result = Arrays.equals((float[]) obj1, (float[]) obj2);
         } else if (obj1 instanceof int[]) {
            result = Arrays.equals((int[]) obj1, (int[]) obj2);
         } else if (obj1 instanceof long[]) {
            result = Arrays.equals((long[]) obj1, (long[]) obj2);
         } else if (obj1 instanceof short[]) {
            result = Arrays.equals((short[]) obj1, (short[]) obj2);
         } else if (obj1 instanceof Object[]) {
            result = Arrays.deepEquals((Object[]) obj1, (Object[]) obj2);
         }
      } else {
         result = (obj1 == obj2 || obj1.equals(obj2));
      }
      return result;
   }

   static {
      try {
         System.loadLibrary("hotrod");
      } catch (UnsatisfiedLinkError e) {
         System.loadLibrary("hotrod32");
      }
      System.loadLibrary("hotrod-jni");
   }
}
