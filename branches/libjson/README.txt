


LIBJSON is a tool to manipulate JSON. The current version is 7.5.1.


Here is the procedure how to put it into Expedia codebase:

1. Download the package from http://sourceforge.net/projects/libjson
2. Generate Expedia makefile to replace the original one
3. Turn off JSON_DEPRECATED_FUNCTIONS in JSONOptions.h
4. Add "#pragma warning(disable:4800)" into files JSONWorker.cpp, libjson.cpp, and internalJSONNode.cpp to disable compiler warnings.


