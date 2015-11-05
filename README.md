[![Build Status](https://magnum.travis-ci.com/matann/fuzzy-database.svg?token=9y2FhEje8Gso8srsgnQj&branch=asio-initial)](https://magnum.travis-ci.com/matann/fuzzy-database)

# fuzzy-database
A graph-based fuzzy data store.

## Building
Initialise biicode with `bii init -L`.

Run `biicode find` to find dependencies, `biicode configure` to download/install dependencies 
and `biicode build` to actually build the solution. The final executable is put in the 'bin' folder. 

Alternatively running `bii buzz` on a clean clone of this branch will do EVERYTHING for you and you'll
 get a binary at the end. 

If you want to generate visual studio files put `-G "Visual Studio 14"` on the `bii buzz` or `bii build` commands.

## Running
The program starts a TCP server on port 1407. Use CTRL-C to kill the running server.