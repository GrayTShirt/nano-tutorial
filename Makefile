ifndef CC
	CC=gcc
endif

CFLAGS=-Wall -g

all: bus
	$(MAKE) run

run:
	/bin/sh -c './bus node0 ipc:///tmp/node0.ipc ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc &'
	/bin/sh -c './bus node1 ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc &'
	/bin/sh -c './bus node2 ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc &'
	/bin/sh -c './bus node3 ipc:///tmp/node3.ipc ipc:///tmp/node0.ipc &'
	sleep 5
	kill -9 $$(ps aux | grep -v grep | grep node | awk '{print $$2}')

%: src/%.c
	rm -f $@
	$(CC) $(CFLAGS) $< -l nanomsg -o $@

clean:
	git clean -xfd

.PHONY: run
