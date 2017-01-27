import logging
import socket
import sys
import time
import os
import subprocess

def main():
    destination = (sys.argv[1], int(sys.argv[2]))
    timeout = int(sys.argv[3])
    if (len(sys.argv)>4):
        if os.name == 'nt' :
            # Hangs on standalone.bat script.  Call Java directly.
            jproc = subprocess.Popen(sys.argv[4], close_fds=True, creationflags=subprocess.CREATE_NEW_CONSOLE);
        else:
            new_env = os.environ.copy()
            # Tell standalone.sh that you want termination signals to get through to the java process
            server_out = open('krbserver.out', 'w')
            jproc = subprocess.Popen(sys.argv[4].split(), stdout=server_out, stderr=server_out, close_fds=True, env=new_env);
            server_out.close()
            
    output = open('krbserver.pkl', 'wb')
    output.close()


    print('Probing %s:%d...' % destination)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    reattempt_delay = 3
    cumulated_time = 0
    attempt = 1
    while True:
        sys.stdout.write('Attempt #%d ' % attempt)
        try:
            s.connect(destination)
            sys.stdout.write('succeded!\n')
            break
        except socket.error:
            logging.exception('Socket connection failed')

            if cumulated_time < timeout:
                sys.stdout.write('Retrying in %ds (%ds to timeout).\n' % (reattempt_delay, timeout - cumulated_time))
                sys.stdout.flush();

                attempt += 1
                cumulated_time += reattempt_delay
                time.sleep(reattempt_delay)

            else:
                sys.stdout.write('Timeout reached. Giving up.\n')
                sys.exit(1);

    s.close()

if __name__ == "__main__":
    sys.exit(main())
