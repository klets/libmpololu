libmpololu
==========

Maestro pololu communication lib for Linux

COMPILING:
   
   make all

USAGE:
   Compile your project with -lmpololu option, see "inc/mpololu.h" for API.
   
   Shared object libmpololu.so will be in lib/ directory

   Executable file mpololu_cmd will be in bin/ directory

EXAMPLE:
   See "src/mpololu_cmd.c" for example usage of API. This util help many options :).
   See multiple targets list format example in "file.txt".
   See "run/run.sh" script for example of usage "mpololu_cmd" util.
   