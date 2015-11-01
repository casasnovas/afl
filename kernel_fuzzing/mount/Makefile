
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

all: mount inputs/vfat

inputs/vfat:
	mkdir -p $(PWD)/inputs
	dd if=/dev/zero of=$(PWD)/inputs/vfat bs=1024 count=34
	mkfs.vfat $(PWD)/inputs/vfat

mount: mount.c ../forkserver.c
	$(CC) -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

run: mount inputs/vfat
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/mount \$${afl_instance} @@"

resume_run: mount inputs/vfat
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances - $(PWD)/outputs "$(PWD)/mount \$${afl_instance} @@"

clean:
	rm -Rf inputs/ mount

.PHONY: clean run resume_run
