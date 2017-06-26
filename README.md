Clairvoyance
============

Sparse graph/matrix visualization tool.

Building
--------

Building Clairvoyance requires a C99 compiler supporting OpenMP specification
3.0 or greater (almost all of my testing is with GCC 4.7 and 4.8) and CMake
2.8 or greater. The configure script requires Bash, however it is possible to
do the configuration by hand. To build Clairvoyance with the default options,
execute:

```
./configure && make
```

Installing
----------

```
make install
```


Usage
-----

Clairvoyance can then be used to generate an image of a graph/matrix as
follows:

```
clairvoyance test.graph test.png
```

Clairvoyance currently has a large number of options, many of which are
experimental. For a complete list, use the -h option:

```
clairvoyance -h
```

