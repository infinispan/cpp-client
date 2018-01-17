# called from CMake/CTest
# usage: python server_ctl start java_exe_path JBOSS_HOME
#        python server_ctl stop

import os
import re
import sys
import string
import subprocess
import time
import pickle

def is_compressed_oops_supported(java):
    try:
        subprocess.check_output([java, '-XX:+UseCompressedOops'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as error:
        # An exception will always be thrown as the cmdline is not complete.
        return (re.search("Unrecognized VM option 'UseCompressedOops'", error.output) is None)
    return true

def static_java_args(java, jboss_home, config, opts):
    # No luck calling standalone.bat without hanging.
    # This is what the batch file does in the default case for 6.0.0.
    cmd = [java, '-XX:+TieredCompilation', '-XX:+UseCompressedOops',
            '-Xms64M', '-Xmx512M', '-XX:MaxPermSize=256M', '-Djava.net.preferIPv4Stack=true',
            '-Djboss.modules.system.pkgs=org.jboss.byteman',
            '-Dorg.jboss.boot.log.file=' + jboss_home + '/standalone/log/server.log',
            '-Dlogging.configuration=file:' + jboss_home + '/standalone/configuration/logging.properties',
            '-jar', jboss_home + '/jboss-modules.jar', '-mp', jboss_home + '/modules',
            '-jaxpmodule', 'javax.xml.jaxp-provider', 'org.jboss.as.standalone', 
            '-Djboss.home.dir=' + jboss_home, '-c', config, opts]
    # This option doesn't work on a 32-bit JVM

    if not is_compressed_oops_supported(java):
        cmd.remove('-XX:+UseCompressedOops')

    return cmd

def start(args):
    java_exe = args[2]
    ispn_server_home = args[3]
    ispn_server_config = args[4]
    ispn_server_opts = "-DNOOP";
    if (len(args)>5) :
        ispn_server_opts = args[5];
    ispn_server_pid_file = "servers.pkl";
    if (len(args)>6) :
        ispn_server_pid_file = args[6];
    stop(["server_ctl.py","stop",ispn_server_pid_file])

    # ctest likes to hang waiting for the subprocesses.  Different
    # tricks on Windows or Linux disassociate the daemon server from
    # ctest.

    if os.name == 'nt' :
        # Hangs on standalone.bat script.  Call Java directly.
        jproc = subprocess.Popen(static_java_args(java_exe, ispn_server_home, ispn_server_config, ispn_server_opts), close_fds=True, creationflags=subprocess.CREATE_NEW_CONSOLE);
    else:
        startup_script = ispn_server_home + '/bin/' + 'standalone.sh';
        new_env = os.environ.copy()
        # Tell standalone.sh that you want termination signals to get through to the java process
        new_env['LAUNCH_JBOSS_IN_BACKGROUND'] = 'yes'
        server_out = open('server.out', 'w')
        jproc = subprocess.Popen([startup_script, '-c', ispn_server_config, ispn_server_opts], stdout=server_out, stderr=server_out, close_fds=True, env=new_env);
        server_out.close()

    output = open(ispn_server_pid_file, 'wb')
    pickle.dump(jproc.pid, output)
    output.close()

    return 0

def stop(args, verbose=False):
    ispn_server_pid_file = "servers.pkl";
    if (len(args)>2) :
        ispn_server_pid_file = args[2];
    if (os.path.exists(ispn_server_pid_file)):
        pkl_file = open(ispn_server_pid_file, 'rb')

        jproc_pid = None
        try:
            jproc_pid = pickle.load(pkl_file)
        except Exception:
            pass

        if jproc_pid is not None:
            if os.name == 'nt' :
                subprocess.call(["taskkill.exe", "/PID", str(jproc_pid), "/F"])
                time.sleep(1)
            else:
                try:
                    subprocess.call(["kill", str(jproc_pid)])
                    time.sleep(1)
                except Exception:
                    pass

        pkl_file.close()
        os.unlink(ispn_server_pid_file)
    else:
        if verbose:
            print('no test servers in use')
    return 0

def main():
    cmd = sys.argv[1]
    if cmd == 'start':
        return start(sys.argv)
    if cmd == 'stop':
        return stop(sys.argv, verbose=True)
    return 1

if __name__ == "__main__":
    sys.exit(main())
