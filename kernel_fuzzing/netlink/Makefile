
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

all: netlink.so inputs

inputs:
	mkdir -p inputs
	./generate_packets.py

netlink.so: netlink.c
	$(CC) -fpic -shared -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

run: netlink.so inputs
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/netlink.so \$${afl_instance} @@"

resume_run: netlink inputs
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances - $(PWD)/outputs "$(PWD)/netlink.so \$${afl_instance} @@"

clean:
	rm -Rf inputs/ netlink.so

.PHONY: clean run resume_run
