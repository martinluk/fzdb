[![Build Status](https://magnum.travis-ci.com/matann/fuzzy-database.svg?token=9y2FhEje8Gso8srsgnQj&branch=asio-initial)](https://magnum.travis-ci.com/matann/fuzzy-database)

# fuzzy-database
A graph-based fuzzy data store.

## Building

It is best to do 'out of source builds'.

Create a directory called build `mkdir build` and set the working directory to that directory.
Then call `cmake ..`, this will download and build dependencies as well as configuring a build
system for the project. Note: call `cmake .. -G "Visual Studio 14"` to build a VS2015 community edition 
project. 

If the command complains that it must be set to DEBUG or release add the option `-DCMAKE_BUILD_TYPE=DEBUG`.

Once the configuration is complete you can either build the project using the generated build files or use the
command `cmake --build .` to get cmake to do that for you.

## Running
The program starts a TCP server on port 1407. Use CTRL-C to kill the running server.
