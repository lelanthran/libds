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

## ds_ll
A linked list implementation; in some cases it is preferable to have a
linked list (fragmented memory) than an array (contiguous memory). This is
a double-linked list to support traversal in both directions:
1. Create a new node using `ds_ll_ins_after()` or `ds_ll_ins_before()` using `NULL` as the previous node or next node respectively. This results in a linked list of a single element.
2. Use the same functions to add new nodes either before or after any known nodes.
3. Use `ds_ll_ins_tail()` and `ds_ll_ins_head()` to add new nodes to the tail or the head of the list, respectively.
4. Use `ds_ll_value()` to return the value (payload) of the node.  5. Use `ds_ll_first()`, `ds_ll_last()`, `ds_ll_next()` and `ds_ll_prev()` to return the first node, last node, next node or previous node respectively. For the `first` and `last` functions, you can pass in any node in the list and the function will traverse the list and return the first/last node as specified.
6. Use the function `ds_ll_remove()` to remove a single node from the list.  This function will leave the rest of the list intact.
7. Use the function `ds_ll_del()` to remove all nodes in the list. Any node in the list can be passed and the function will traverse the entire list from that node, removing all nodes in the list.
