CC = cc
CFLAGS = -ggdb -Wall -Wextra

PROTO = $(shell pkg-config --variable=pkgdatadir wayland-protocols)
XDG_XML = $(PROTO)/stable/xdg-shell/xdg-shell.xml
XDG_CFLAGS = $(shell pkg-config --cflags wayland-client)

.PHONY: all examples clean

all: examples bin/miniwindow.a 

bin/miniwindow.a: bin-int/miniwindow.o bin-int/xdg-shell-protocol.o
	mkdir -p $(@D)
	ar -rcs $@ $^

bin-int/miniwindow.o: miniwindow.c miniwindow.h
	mkdir -p $(@D)
	$(CC) -c $(CFLAGS) miniwindow.c -o $@ -DMINIWINDOW_WAYLAND

bin-int/xdg-shell-protocol.o: generated/xdg-shell-protocol.c
	mkdir -p $(@D)
	$(CC) -c $< -o $@

generated/xdg-shell-protocol.c:
	mkdir -p $(@D)
	wayland-scanner private-code $(XDG_XML) $@ $(XDG_CFLAGS)

examples: bin/miniwindow.a
	make -C examples/hello_miniwindow

clean:
	rm -rf generated
	rm -rf bin
	rm -rf bin-int
