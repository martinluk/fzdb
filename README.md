# fuzzy-database
A graph-based fuzzy data store.

## Building
Run `biicode find` to find dependencies, `biicode configure` to download/install dependencies 
and `biicode build` to actually build the solution. The final executable is put in the 'bin' folder. 

By default the system will attempt to build using the 2015 Visual C++ compiler which is available
in Visual Studio Community Edition 2015 that can be downloaded for free from the Microsoft website.

## Running
The program starts a TCP server on port 1407. Use CTRL-C to kill the running server.