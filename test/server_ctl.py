# called from CMake/CTest
# usage: python server_ctl start java_exe_path ISPN_HOME sigle|multi
#       python server_ctl stop

import os
import sys
import string
import subprocess
import time
import pickle

def expandcp(jars, ispn_home, module_file):
    if os.name == 'nt' :
        sep = ';' # windows
    else:
        sep = ':'
    jarlist = open(ispn_home + module_file, 'r').read()
    newjars = string.split(jarlist, ':')
    cp = ''
    for j in newjars:
        jar = string.replace(j, '$ISPN_HOME', ispn_home)
        if jar not in jars :
            jars[jar] = 1
            if cp:
                cp += sep
            cp += jar
    return cp

def start(args):
    stop(verbose=False)
    java_exe = args[2]
    ispn_home = args[3]
    type = args[4]
    if not os.path.isdir(ispn_home + '/modules') :
        raise RuntimeError('bad infinipsan root directory ' + ispn_home)
    classpath=''
    jars=dict()
    if os.name == 'nt' :
        sep = ';' # windows
    else:
        sep = ':'

    for dir in ['memcached', 'hotrod', 'websocket', 'cli-server'] :
        cp = expandcp(jars, ispn_home, '/modules/' + dir + '/runtime-classpath.txt')
        if cp :
            classpath += sep + cp
        
    classpath = ispn_home + '/modules/cli-server/infinispan-cli-server.jar' + sep + \
                ispn_home + '/modules/websocket/infinispan-server-websocket.jar' + sep + \
                ispn_home + '/modules/hotrod/infinispan-server-hotrod.jar' + sep + \
                ispn_home + '/modules/memcached/infinispan-server-memcached.jar' + \
                classpath
    log4j_config='-Dlog4j.configuration=file:///' + ispn_home + '/etc/log4j.xml'

    jargs = [java_exe, '-classpath', classpath, '-Dcom.sun.management.jmxremote.ssl=false', 
             '-Dcom.sun.management.jmxremote.authenticate=false', '-Dcom.sun.management.jmxremote.port=2488',
             '-Djgroups.bind_addr=127.0.0.1', '-Djava.net.preferIPv4Stack=true', log4j_config,
             '-Dsun.nio.ch.bugLevel=""', 'org.infinispan.server.core.Main', '-r', 'hotrod']

    # ctest likes to hang on these daemon hotrod servers.  Different
    # tricks on Windows or Linux prevent that

    if os.name == 'nt' :
        jproc = subprocess.Popen(jargs,close_fds=True, creationflags=subprocess.CREATE_NEW_CONSOLE);
    else:
        server_out = open('server.out', 'w')
        jproc = subprocess.Popen(jargs,stdout=server_out, stderr=server_out, close_fds=True);
        server_out.close()

    output = open('servers.pkl', 'wb')
    if os.name == 'nt' :
        # Windows can't pickle the process, so just save the pid
        pickle.dump(jproc.pid, output)
    else:
        pickle.dump(jproc, output)
    output.close()

    # TODO: better check that the server has had time to initialize, but for now...
    time.sleep(3)

    return 0

def stop(verbose=False):
    if (os.path.exists('servers.pkl')):
        pkl_file = open('servers.pkl', 'rb')
        if os.name == 'nt' :
            jproc_pid = pickle.load(pkl_file)
            subprocess.call(["taskkill.exe", "/PID", str(jproc_pid), "/F"])
            time.sleep(1)
        else:
            jproc = pickle.load(pkl_file)
            try:
                jproc.terminate()
                time.sleep(1)
            except Exception:
                pass

        pkl_file.close()
        os.unlink('servers.pkl')
    else:
        if verbose:
            print 'no test servers in use'
    return 0

def main():
    cmd = sys.argv[1]
    if cmd == 'start':
        return start(sys.argv)
    if cmd == 'stop':
        return stop(verbose=True)
    return 1

if __name__ == "__main__":
    sys.exit(main())
