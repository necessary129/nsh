# NSH: Not(e) (A) Shell

NSH is a (pseudo) shell written for OSN Assignment 2 at IIIT Hyderabad

## Building

Make sure you have `cmake` installed. 

First, Create a build directory:

```
mkdir build
cd build
```

Run `cmake`, with debug flag for debugging or release flag otherwise.

```
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Then, either run `make` or `ninja`:

```
make
```
or

```
ninja -C
```

You should have the shell in `nsh` in the `build/bin/` directory.

## Commands and features

- ### `cd`
Changes the directory.

- ### `pwd`
Prints the current directory

- ### `echo`
Prints the arguments

- ### `discover`
Rudimentary `find` equivalent.

- ### `history`
Prints the last 10 commands entered in shell.

## Quirks

1. Home directory is defined as the directory where the shell is initialized: Assignment specification.
1. Argument parsing is very rudimentary.
1. Environment variables are not touched: Assignment specification(?)
1. Job id's are always increasing.