# Data-structures
## A collection of data structures for C99

Some data structures for generic data. Copy the files you need into your project and build. The sources are intended to be more comprehensive than a github gist and less fiddly than a proper library.

These data structures are for small (in-memory) data. For large data use a library that serialises to disk/db/server. This library only provides the containers. The elements stored in these containers are not automatically deleted and/or created. THe caller must create data objects (such as strings), store them in the container data structure, and free those data objects when needed.

## ds_array
1. A simple linear array.
2. Getting the length is expensive: don't do it often (in a loop, for example).
3. The elements can be directly accessed via the array of void pointers.
