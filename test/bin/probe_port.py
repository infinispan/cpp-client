import socket
import sys
import time

def main():
    destination = (sys.argv[1], int(sys.argv[2]))
    timeout = int(sys.argv[3])

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
        except socket.error as ex:
            sys.stdout.write('failed with "%s". ' % ex)

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
