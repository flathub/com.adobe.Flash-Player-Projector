all: libfix-config-location.so

fix-config-location.o: fix-config-location.c
	gcc -Wall -fPIC -c -o fix-config-location.o fix-config-location.c

libfix-config-location.so: fix-config-location.o
	gcc -shared -fPIC -Wl,-soname -Wl,libfix-config-location.so -o libfix-config-location.so fix-config-location.o

install: libfix-config-location.so
	install -m755 -Dt /app/lib libfix-config-location.so
