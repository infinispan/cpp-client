import logging
import socket
import sys
import time

def main():
    destination = (sys.argv[1], int(sys.argv[2]))
    timeout = int(sys.argv[3])
    isDown = (len(sys.argv) >4) and (sys.argv[4]== 'down')
    print('Probing %s:%d...' % destination)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    reattempt_delay = 3
    cumulated_time = 0
    attempt = 1
    if isDown is False:
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
    else:
        while True:
            sys.stdout.write('Attempt #%d ' % attempt)
            try:
                s.connect(destination)
                logging.exception('Socket connection still up')
                s.close()

                if cumulated_time < timeout:
                    sys.stdout.write('Retrying in %ds (%ds to timeout).\n' % (reattempt_delay, timeout - cumulated_time))
                    sys.stdout.flush();

                    attempt += 1
                    cumulated_time += reattempt_delay
                    time.sleep(reattempt_delay)

                else:
                    sys.stdout.write('Timeout reached. Giving up.\n')
                    sys.exit(1);
            except socket.error:
                sys.stdout.write('succeded!\n')
                break

if __name__ == "__main__":
    sys.exit(main())
