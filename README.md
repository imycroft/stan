# Static Analysis Tool
This tool has been developped to help extracting features from files
## Compilation instructions
### On Linux
```sh
gcc static.c stan.c -o stan.out -lm
```
### On Windows
Install **gcc** for [windows](https://sourceforge.net/projects/mingw/), add the environment variable in your PATH, then in a CLI type: <br> 
```sh
gcc static.c stan.c -o stan
```
#### Compile with DLL
First create the object code <br>
```sh
gcc -c static.c
```
then, build the shared library <br>
```sh
gcc -shared -o static.dll static.o
```
finally, build the executable <br>
```sh
gcc -o stan stan.c -L./ -lstatic
```
## How to use the DLL in your program
include the **static.h** file your code <br>
```C
#include "static.h"
```
After including the header, you can use any function declared inside. <br>
compile your program with the command: <br>
```sh
gcc -o yourProgram yourCode.c -L./ -lstatic 
```
## How to use the program
```sh
stan -h
```
