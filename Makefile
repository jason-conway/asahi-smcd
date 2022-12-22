.POSIX:
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -Os
LDFLAGS =
LDLIBS  =

asahi-smcd: asahi-smcd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ asahi-smcd.c $(LDLIBS)

install: asahi-smcd asahi-smcd.service
	install -Dm755 asahi-smcd /usr/bin/asahi-smcd
	install -Dm664 asahi-smcd /etc/systemd/system/asahi-smcd.service

uninstall:
	rm -f /usr/bin/asahi-smcd
	systemctl stop asahi-smcd.service
	systemctl disable asahi-smcd.service
	rm -f /etc/systemd/system/asahi-smcd.service
	systemctl daemon-reload
	systemctl reset-failed

clean:
	rm -f asahi-smcd
