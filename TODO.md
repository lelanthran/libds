# TODO list

## Key/value storage

Lots of projects require storage of unique keys and values without needing the
complexity of using JSON to store these. The problem with trying to use JSON
for this is that duplicates are allowed in JSON.

A simpler object that stores values for a key (creating key if it doesn't
exist and overwriting it if it does) is better. Can use the `ds_hmap_t` object
to act as the backing store, or even simpler, store it in an array and have
linear access times.

The added value for this would be coercing every `value` into a string object,
and coercing every string object back into a value.



## ds_json_t

### Destructors

Destructor `ds_json_del()` must not delete instance if instance is attached to
a parent object.

This requires adding in and maintaining a pointer to parent, and maybe adding
in a functions `ds_json_object_detach()` and `ds_json_array_detach()` to
detach without deletion objects attached to objects and arrays.

A potentially quicker, but error-prone, way is to make the append functions
take the address of a pointer to the ds_json_t instance that must be appended.
That way they can set the caller's pointer to the ds_json_t instance to NULL
when they successfully attach the given instance.

**Alternative** Add in a `parent` pointer to the `ds_json_t` object. When
deleting, we can always manually remove the instance by chasing the `parent`
pointer.

### Change `_new()` functions

The `_object_new()` and `_array_new()` functions must take parameters, to
allow inline/chained creation (and so must also do sensible things when given
NULL values).

Might be better to make a chainable function for this than to modify the
existing functions.


