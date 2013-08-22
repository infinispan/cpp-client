# this should be in cmake or python for portability
# run from your build directory

# set this
ISPN_HOME=/opt/infinispan-5.3.0.Final-all/

die()
{
  echo $* >&2
  exit 1
}

[ -d $ISPN_HOME ] || die canpot find ispn home
[ -f CMakeCache.txt ] || die cmake not run yet
hrsrc=`grep HotRod_SOURCE_DIR: CMakeCache.txt | sed 's/.*=//'`

hr_jar=$ISPN_HOME/modules/hotrod-client/infinispan-client-hotrod.jar
#hr_xtra=`cat $ISPN_HOME/modules/hotrod-client/runtime-classpath.txt`
hr_xtra=$hrsrc/test/lib/*
#hr_xtra=`eval echo $hr_xtra`

sources=`find $hrsrc/test/jniapi test/swig -name '*.java'`

rm -rf jnitmp
mkdir jnitmp

javac -cp "jnitmp:$hr_xtra" -d jnitmp $sources || die javac failed for jni library
(cd jnitmp; jar cf hotrod-jni.jar org)

cp ./test/swig/libhotrod-jni.so jnitmp

javac -cp "jnitmp/hotrod-jni.jar:$hr_xtra" -d jnitmp $hrsrc/test/JniTest.java || die javac failed for test program


# These two identical except LD_LIBRARY_PATH and s/infinispan-client-hotrod.jar/hotrod-jni.jar/ 
echo java -ea -cp "$hr_jar:$hr_xtra:." Simple2 >jnitmp/run_java.sh
echo LD_LIBRARY_PATH=. java -ea -cp "hotrod-jni.jar:$hr_xtra:." JniTest >jnitmp/run_jni.sh

chmod u+x ./jnitmp/run_j*

echo success
echo chdir to jnitmp and run "sh run_java.sh" or "sh run_jni.sh"
