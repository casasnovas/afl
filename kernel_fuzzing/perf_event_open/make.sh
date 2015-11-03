#! /bin/bash

set -e
set -u

function compile {
	gcc -std=gnu99 -O3 -Wall -DMODE_INIT=1 -DMODE_RUN=2 $@ main.c
}

compile -DMODE=MODE_INIT -o init -DVERBOSE
compile -DMODE=MODE_RUN -o run_afl
compile -DMODE=MODE_RUN -o run -DVERBOSE
