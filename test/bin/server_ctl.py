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
import shutil

def is_compressed_oops_supported(java):
    try:
        subprocess.check_output([java, '-XX:+UseCompressedOops'], stderr=subprocess.STDOUT)
    except subprocess.CalledProcessError as error:
        # An exception will always be thrown as the cmdline is not complete.
        return (re.search("Unrecognized VM option 'UseCompressedOops'", error.output) is None)
    return true

def static_java_args(java, jboss_home, config, opts):
    # No luck calling server.bat without hanging.
    # This is what the batch file does in the default case for 6.0.0.
    cmd = [jboss_home+"/bin/server.bat",
            '-c', config]+ opts.split()
    print (cmd)

    return cmd

def start(args):
    java_exe = args[2]
    ispn_server_home = args[3]
    ispn_server_config = args[4]
    ispn_server_opts = "-DNOOP";
    if (len(args)>5):
        ispn_server_opts = args[5];
    ispn_server_pid_file = "servers.pkl";
    if (len(args)>6):
        ispn_server_pid_file = args[6];
    server_data_home = "server";
    if (len(args)>7):
        server_data_home = args[7];

    stop(["server_ctl.py","stop",ispn_server_pid_file])

    # ctest likes to hang waiting for the subprocesses.  Different
    # tricks on Windows or Linux disassociate the daemon server from
    # ctest.

    if os.name == 'nt' :
        # Hangs on server.bat script.  Call Java directly.
        if (os.path.isdir(ispn_server_home+"/"+server_data_home+"/data")) :
            shutil.rmtree(ispn_server_home+"/"+server_data_home+"/data")
        jproc = subprocess.Popen(static_java_args(java_exe, ispn_server_home, ispn_server_config, ispn_server_opts), close_fds=True, creationflags=subprocess.CREATE_NEW_CONSOLE);
    else:
        if (os.path.isdir(ispn_server_home+"/"+server_data_home+"/data")) :
            shutil.rmtree(ispn_server_home+"/"+server_data_home+"/data")
        startup_script = ispn_server_home + '/bin/' + 'server.sh';
        new_env = os.environ.copy()
        # Tell server.sh that you want termination signals to get through to the java process
        new_env['LAUNCH_JBOSS_IN_BACKGROUND'] = 'yes'
        server_out = open('server.out', 'w')
        jproc = subprocess.Popen([startup_script, '-c', ispn_server_config]+ispn_server_opts.split(), stdout=server_out, stderr=server_out, close_fds=True, env=new_env);
        server_out.close()

    output = open(ispn_server_pid_file, 'wb')
    pickle.dump(jproc.pid, output)
    output.close()

    return 0

def stop(args, verbose=False):
    ispn_server_pid_file = "servers.pkl";
    if (len(args)>2):
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
                try:
                    subprocess.call(["taskkill.exe", "/PID", str(jproc_pid), "/F", "/T"])
                    time.sleep(1)
                except Exception:
                    pass
            else:
                try:
                    subprocess.call(["pkill", "-P", str(jproc_pid)])
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
