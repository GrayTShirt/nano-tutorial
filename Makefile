
all: bus

bus:
	rm -f bus
	gcc -g -Wall src/bus.c -l nanomsg -o bus
	$(MAKE) run

run:
	/bin/sh -c './bus node0 ipc:///tmp/node0.ipc ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc & node0=$$!'
	/bin/sh -c './bus node1 ipc:///tmp/node1.ipc ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc & node1=$$!'
	/bin/sh -c './bus node2 ipc:///tmp/node2.ipc ipc:///tmp/node3.ipc & node2=$$!'
	/bin/sh -c './bus node3 ipc:///tmp/node3.ipc ipc:///tmp/node0.ipc & node3=$$!'
	sleep 5
	kill -9 $$(ps aux | grep -v grep | grep node | awk '{print $$2}')

.PHONY: run
.PHONY: bus
