# Data-structures
## A collection of small data structures for C99

Some data structures for generic data. Copy the files you need into your project and build. The sources are intended to be more comprehensive than a github gist and less fiddly than a proper library.

These data structures are for small (in-memory) data. For large data use a library that serialises to disk/db/server. This library only provides the containers. The elements stored in these containers are not automatically deleted and/or created. The caller must create data objects (such as strings), store them in the container data structure, and free those data objects when needed.

## ds_array
1. A simple linear array.
2. Getting the length is expensive: don't do it often (in a loop, for example). If the length is needed often, store the length in a separate variable and update it on every insertion/removal.
3. Cannot store NULL pointers - a NULL signifies the end of the array.
4. The elements can be directly accessed via the array of void pointers.
5. As a result, looping across the array is as simple as:

        void *array = ds_array_new ();
        ... // Insert elements
        for (size_t i=0; array[i]; i++) {
            ... // Use array[i]
        }

## ds_str
Functions for perfoming:
1. String copy (with allocation),
2. String concatenation (with allocation) resulting in a new string,
3. String appending to an existing string (with reallocation of existing
   string).
