
PWD=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/

all: loader inputs/tiny

inputs/tiny: tiny.s
	mkdir -p $(PWD)/inputs
	$(CC) $< -nostdlib -o $@

loader: loader.c ../forkserver.c
	$(CC) -I $(PWD)/../../ -I $(PWD)/../ $^ -o $@

run: loader inputs/tiny
	../scripts/start_instances $(PWD)/inputs $(PWD)/outputs "$(PWD)/loader @@"

resume_run: loader inputs/tiny
	../scripts/start_instances - outputs "$(PWD)/mount @@"

clean:
	rm -Rf inputs/ loader

.PHONY: clean run resume_run
