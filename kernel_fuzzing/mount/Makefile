
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

filesystems=vfat iso9660
inputs=$(addprefix inputs/,$(filesystems))

all: mount.so $(inputs)

inputs/iso9660:
	mkdir -p $(PWD)/inputs; \
		temp_dir=$$(mktemp -d /tmp/isofsXXXX) ; \
		echo foo > $${temp_dir}/foo ; \
		echo bar > $${temp_dir}/bar ; \
		mkdir dir > $${temp_dir}/dir; \
		echo blah > $${temp_dir}/dir/blah; \
		genisoimage -o $@ $${temp_dir}; \
		rm -Rf $${temp_dir}; \

inputs/vfat:
	mkdir -p $(PWD)/inputs
	dd if=/dev/zero of=$(PWD)/inputs/vfat bs=1024 count=34
	mkfs.vfat $(PWD)/$@

mount.so: mount.c
	$(CC) -fpic -shared -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

define one_fs
run_$(1): mount.so inputs/$(1)
	AFL_FUZZ=$$(PWD)/../../afl-fuzz ../scripts/start_instances $$(PWD)/inputs $$(PWD)/outputs "$$(PWD)/mount.so $1 \$$$${afl_instance} @@"
resume_$(1): mount.so inputs/$(1)
	AFL_FUZZ=$$(PWD)/../../afl-fuzz ../scripts/start_instances - $$(PWD)/outputs "$$(PWD)/mount.so $1 \$$$${afl_instance} @@"
endef

$(foreach fs,$(filesystems),$(eval $(call one_fs,$(fs))))

clean:
	rm -Rf inputs/ mount mount.so

.PHONY: clean run resume_run
