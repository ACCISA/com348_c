#!/usr/bin/bash
clear
gcc -Wall aggregate.c singular.c mathpipe.c -o mathpipe
cat input.txt | ./mathpipe -size=1 FILTER GEQ 10 -prec=0 | ./mathpipe SUM