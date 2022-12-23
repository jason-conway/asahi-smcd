.POSIX:
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Os

asahi-smcd: asahi-smcd.c
	$(CC) $(CFLAGS) -o $@ asahi-smcd.c

install: asahi-smcd asahi-smcd@.service
	install -Dm755 asahi-smcd /usr/bin/asahi-smcd
	install -Dm664 asahi-smcd@.service /etc/systemd/system/asahi-smcd@.service

uninstall:
	rm -f /usr/bin/asahi-smcd

clean:
	rm -f asahi-smcd
