
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

all: loader.so inputs/tiny

inputs/tiny: tiny.s
	mkdir -p $(PWD)/inputs
	$(CC) $< -nostdlib -o $@

loader.so: loader.c
	$(CC) -fpic -shared -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

run: loader.so inputs/tiny
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/loader.so @@"

resume_run: loader.so inputs/tiny
	AFL_FUZZ=$(PWD)/../../afl-fuzz ../scripts/start_instances - outputs "$(PWD)/loader.so @@"

clean:
	rm -Rf inputs/ loader loader.so

.PHONY: clean run resume_run
