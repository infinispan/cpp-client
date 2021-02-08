SCRIPT_PATH=$(dirname $(readlink -f $0))
if [ -z "$SERVER_ZIP" ]
then
	echo SERVER_ZIP variable must point to the server zip file
else
RPM_PATH=${RPM_PATH:-$HOME/git/cpp-client/prebuilt-tests/rpm}
TEST_SRC_PATH=${TEST_SRC_PATH:-$HOME/git/cpp-client/}

podman run -v `dirname "$SERVER_ZIP"`:/home/jboss/zip -v $RPM_PATH:/home/jboss/inst -v $TEST_SRC_PATH:/home/jboss/cpp-client -v $SCRIPT_PATH/image:/home/jboss/scripts -e SERVER_ZIP=`basename "$SERVER_ZIP"` -it cpp-test-machine-c7:latest /home/jboss/scripts/run-tests.sh


#podman run -v `dirname "$SERVER_ZIP"`:/home/jboss/zip -v /home/rigazilla/git/cpp-client/prebuilt-tests/:/home/jboss/inst -v /home/rigazilla/git/cpp-client:/home/jboss/cpp-client -v $SCRIPT_PATH/image:/home/jboss/scripts -e SERVER_ZIP=infinispan-server-11.0.4.Final.zip -it cpp-test-machine-c7:latest /home/jboss/scripts/run-tests.sh
fi
