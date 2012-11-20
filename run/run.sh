#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/limpololu/lib
bin/mpololu_cmd  --mult-num 3 --mult-first 0 --file file.txt