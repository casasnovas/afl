
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

inputs= inputs/vfat

all: mount.so $(inputs)

inputs/vfat:
	mkdir -p $(PWD)/inputs
	dd if=/dev/zero of=$(PWD)/inputs/vfat bs=1024 count=34
	mkfs.vfat $(PWD)/inputs/vfat

mount.so: mount.c
	$(CC) -fpic -shared -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

define one_fs
run_$(1): mount.so inputs/$1
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/mount.so $1 \$${afl_instance} @@"
resume_$(1): mount.so inputs/$1
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances - $(PWD)/outputs "$(PWD)/mount.so $1 \$${afl_instance} @@")
endef

$(call one_fs, vfat)

# run_vfat: mount.so inputs/vfat
# 	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/mount.so vfat \$${afl_instance} @@"

# resume_vfat: mount inputs/vfat
# 	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances - $(PWD)/outputs "$(PWD)/mount.so vfat \$${afl_instance} @@"

clean:
	rm -Rf inputs/ mount mount.so

.PHONY: clean run resume_run
