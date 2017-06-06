# File IO Code Sample

This code sample was made to solve the hypothetical problem of an IO bottleneck when writing data to a slow peripheral storage drive, represented here by writing byte arrays to a simple local file. It was designed and revised iteratively to simulate changing requirements with the following constraints in mind:

* Start with a set of C functions for opening streams and writing data
* Create a class-based C++ interface and a stream manager
* Add the ability to write metainformation
* Extend with simple threading
* Include minimal but specific tests (see main.cpp)
