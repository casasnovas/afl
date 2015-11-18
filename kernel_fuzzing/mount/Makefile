
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

filesystems=vfat iso9660 btrfs
inputs=$(addsuffix /fs,$(addprefix inputs_,$(filesystems)))

all: mount.so $(inputs)

inputs_%:
	mkdir -p $@

inputs_iso9660/fs:
	temp_dir=$$(mktemp -d /tmp/isofsXXXX) ; \
		echo foo > $${temp_dir}/foo ; \
		echo bar > $${temp_dir}/bar ; \
		mkdir $${temp_dir}/dir; \
		echo blah > $${temp_dir}/dir/blah; \
		genisoimage -o $@ $${temp_dir}; \
		rm -Rf $${temp_dir}; \

inputs_vfat/fs:
	dd if=/dev/zero of=$@ bs=1024 count=34
	mkfs.vfat $@

inputs_btrfs/fs:
	sudo python btrfs.py $@

mount.so: mount.c
	$(CC) -Wall -std=gnu99 -fpic -shared -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

define one_fs
run_$(1): mount.so inputs_$(1)/fs
	AFL_FUZZ=$$(PWD)/../../afl-fuzz ../scripts/start_instances $$(PWD)/inputs_$1/ $$(PWD)/outputs_$1 "$$(PWD)/mount.so $1 \$$$${afl_instance} @@"

resume_$(1): mount.so inputs_$(1)/fs
	AFL_FUZZ=$$(PWD)/../../afl-fuzz ../scripts/start_instances - $$(PWD)/outputs_$1 "$$(PWD)/mount.so $1 \$$$${afl_instance} @@"

inputs_$1/fs: inputs_$1
endef

$(foreach fs,$(filesystems),$(eval $(call one_fs,$(fs))))

clean:
	rm -Rf $(foreach fs,$(inputs),$(dir $(fs))) mount.so

.PHONY: clean run resume_run
