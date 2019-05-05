# Data-structures
## A collection of small data structures for C99

Some data structures for generic data. Copy the files you need into your
project and build. The sources are intended to be more comprehensive
than a github gist and less fiddly than a proper library.

These data structures are for small (in-memory) data. For large data
use a library that serialises to disk/db/server. This library only
provides the containers. The elements stored in these containers are
not automatically deleted and/or created. The caller must create data
objects (such as strings), store them in the container data structure,
and free those data objects when needed.

## Dynamic array implementation - ds_array
1. A simple linear array.
2. Getting the length is expensive: don't do it often (in a loop, for
   example). If the length is needed often, store the length in a separate
   variable and update it on every insertion/removal.
3. Cannot store NULL pointers - a NULL signifies the end of the array.
4. The elements can be directly accessed via the array of void pointers.
5. As a result, looping across the array is as simple as:

        void *array = ds_array_new ();
        ... // Insert elements
        for (size_t i=0; array[i]; i++) {
            ... // Use array[i]
        }

## Useful string functions - ds_str
Functions for performing:
1. String copy (with allocation),
2. String concatenation (with allocation) resulting in a new string,
3. String appending to an existing string (with reallocation of existing
   string).
4. Dynamic allocation printf; use printf and print into a buffer that is
   allocated by the `ds_str_printf()` function to be large enough for the
   entire string. The caller must free the allocated buffer.

## Double linked-list implementation - ds_ll
A linked list implementation; in some cases it is preferable to have a
linked list (fragmented memory) than an array (contiguous memory). This is
a double-linked list to support traversal in both directions:
1. Create a new node using `ds_ll_ins_after()` or `ds_ll_ins_before()`
   using `NULL` as the previous node or next node respectively. This results
   in a linked list of a single element.
2. Use the same functions to add new nodes either before or after any
   known nodes.
3. Use `ds_ll_ins_tail()` and `ds_ll_ins_head()` to add new nodes to
   the tail or the head of the list, respectively.
4. Use `ds_ll_value()` to return the value (payload) of the node.  5. Use
   `ds_ll_first()`, `ds_ll_last()`, `ds_ll_next()` and `ds_ll_prev()`
   to return the first node, last node, next node or previous node
   respectively. For the `first` and `last` functions, you can pass in any
   node in the list and the function will traverse the list and return the
   first/last node as specified.
6. Use the function `ds_ll_remove()` to remove a single node from
   the list.  This function will leave the rest of the list intact.
7. Use the function `ds_ll_del()` to remove all nodes in the list. Any
   node in the list can be passed and the function will traverse the entire
   list from that node, removing all nodes in the list.

## General hashmap implementation - ds_hmap
_(Note - In this section, a type of `string` means a `char *` that is
nul-terminated and compatible with all of the string functions in the
standard `C` library)._

### Overview
The caller will always give the hashmap two elements of data - a `key` and
a `value` associated with that `key`. Of these two elements of data passed
to the hashmap:
1. The hashmap makes a copy of the `key` that it is given. This allows the
   caller to use local variables as the `key` so that even when they go
   out of scope the value stored by the hashmap is still valid.
2. The hashmap _does not_ make a copy of the data passed to it. This is
   because the use of the hashmap is to quickly locate a specific instance
   of a data object, not an equivalent (which is what a copy will be).

It is important to understand that when a hash of (for example)
`{ "MyKeyVal", &data_object }` is stored, we don't care if we later use
a different instance of a `string` equivalent to `"MyKeyVal"` to find the
object, as long as the new instance compares the same, but we __do__ care
to get the same instance of `data_object` that was stored, not a copy of
it!

### Convenience functions
This hashmap implementation stores variable-length keys associated with
variable-length data. Due to the generality of the storage, the interface
requires a pointer _as well as_ a length indicator for both keys and data.
A few convenience functions exist for storing common hashes of
`<string, string>` and `<string, void *>`:
   - `ds_hmap_set_str_str()`
      Set a hash using a key of type `string` and a value of type
      `string`.
   - `ds_hmap_get_str_str()`
      Get a hash using a key of type `string` returning value of type
      `string`.
   - `ds_hmap_remove_str()`
      Remove a hash using a key of type `string`.
   - `ds_hmap_set_str_ptr()`
      Set a hash using a key of type `string` and a value of a
      size-indicated buffer.
   - `ds_hmap_get_str_ptr()`
      Get a hash using a key of type `string` returning value of a
      size-indicated buffer.

### Bucket length
When creating the hashmap with `ds_hmap_new()`, specify a number of
buckets that is approximately 75% as large as the number of elements you
may want to store. For example, if you expect to store 100 elements, use a
bucket number of `75`.

If you are unsure of how many elements you may potentially store use a number
that scales with your uncertainty. For example, if you are only 50%
certain that you will store 100 elements, use 150 as the number of
buckets.

Do not worry if you specify a bucket number that is too small. You will not
lose any data (all hashes will still be stored); the only difference is that
performance of searching/inserting into the hashmap will slow down as the
number of elements inserted grows.

Specifying a bucket number that is too large will potentially waste space
but the wasted space is extremely small. Only 8 bytes will be wasted for each
unused bucket. Thus, for example, using a bucket length of 1000 when you
only needed 200 results in wasted space of 6.25Kb. If the 200 elements are
a non-trival `class` or `struct`, they will take up at least 4.5Kb by
themselves, so even though you over-specified by 4X, the space being
wasted is only around ~1.3X of the data used.

### Coming soon
#### Rehashing / Changing the number of buckets
This is currently not possible. Added more buckets means that the keys now
hash to different values and that the whole hashmap must be rehashed with
a new hash being calculated for every entry.

Hopefully I will have time in the future to add in a function that creates
a new hashmap with a larger bucket number, then inserts every `{key, value}`
element from the existing hashmap into the new one and finally deletes the
old one. This will be the only way to increase the number of buckets in
the hashmap.

#### Shrinking the hashmap / removing elements
Currently the `ds_hmap_remove()` function does not reclaim the space used
by the `{key, value}` element that is being removed. All that happens is
that the element is marked as deleted, and is reused whenever possible.
In an active hashmap, when items are often removed and other items are
often added, most of the removed elements are simply reused.

If time permits, I will add a function that reclaims all the space being
used by elements that were removed and not reused.
