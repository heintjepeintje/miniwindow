.PHONY: clean

bin/main: main.c
	mkdir -p $(@D)
	cc -ggdb -Wall -Wextra main.c -o bin/main -I../../ -L../../bin -l:miniwindow.a -lwayland-client

clean:
	rm -rf bin
