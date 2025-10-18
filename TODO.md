
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

### Change `_new()` functions

The `_object_new()` and `_array_new()` functions must take parameters, to
allow inline/chained creation (and so must also do sensible things when given
NULL values).

Might be better to make a chainable function for this than to modify the
existing functions.

