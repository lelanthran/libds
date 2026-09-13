
#ifndef H_DS_LL
#define H_DS_LL

#include <stdlib.h>

typedef struct ds_array_t ds_array_t;

// This array stores pointers to objects that must be allocated and freed by the caller.
// Removing an entry from the array does not free the object stored by the caller. The caller
// must free all objects that they have allocated.

#ifdef __cplusplus
extern "C" {
#endif

// Create a new array, returning NULL on error. The returned value must be
// freed by the caller using `ds_array_del()`.
ds_array_t *ds_array_new (void);

// Free an array returned with `ds_array_new`. Parameter `ll` can be NULL.
// Note that only the array is freed; the elements of the array are untouched
// by this function.
void ds_array_del (ds_array_t *ll);

// Make a copy of the array. Returns a new ds_array_t on success, NULL on
// error. Note that the returned value will be NULL if the bounds
// specified would result in a zero or negative copy. The src array is bounds
// checked when copying the pointers into the dst array.
// The returned value must be freed by the caller using `ds_array_del()`. Note
// that the returned array will have elements shared with the `src` array.
ds_array_t *ds_array_copy (const ds_array_t *src, size_t from_index, size_t to_index);

// Return the length of the array, which may be `0` in the case of an empty
// array. Note that a NULL `ll` parameter also returns a zero length.
size_t ds_array_length (const ds_array_t *ll);

// Returns the pointer stored at position `i` in the array, or NULL if the
// parameter `ll` is NULL or if the position is out of bounds on the `ll`
// array. Note that if a NULL is legitimately stored at position `i`, it will
// be returned and the caller will not be able to distinguish an element of
// value NULL being returned and an error.
void *ds_array_get (const ds_array_t *ll, size_t i);

// Calls the specified function `fptr` on each element of the array in the
// order from first element to last element. The parameter `fptr` is a
// function that receives the pointer stored in the array, and the extra
// parameter `param` that is passed to all invocations of `fptr`. This is to
// facilitate passing (for example) a `FILE *` when printing out nodes, or
// similar. If either `ll` or `fptr` is NULL, no action is taken.
void ds_array_iterate (const ds_array_t *ll,
                       void (*fptr) (void *, void *), void *param);

// Same as `ds_array_iterate()`, but the iteration is performed starting at
// the last element and ending with the first one. If either `ll` or `fptr` is
// NULL, no action is taken.
void ds_array_iterate_reverse (const ds_array_t *ll,
                               void (*fptr) (void *, void *), void *param);

// Inserts an element into the array, moving all elements after `pos`
// forward by one to make space for the new element. On success returns the
// element inserted. On failure returns NULL. If `pos` is out of bounds, the
// new element is inserted at the end of the array, same as calling
// `ds_array_ins_tail (ll, el)`.
// Note that it is possible to store a NULL value in the array, and retrieval
// will retrieve the NULL value.
void *ds_array_ins (ds_array_t *ll, void *el, size_t pos);

// Appends an element to the array. Returns the element just added on success,
// or NULL if an OOM error occurred, or if array is NULL.
// Note that it is possible to store a NULL pointer in the array, but in that
// case the caller must use `ds_array_length()` to determine success as
// `ds_array_ins_tail()` will return the pointer just added, i.e. NULL.
void *ds_array_ins_tail (ds_array_t *ll, void *el);

// Inserts an element at the beginning of the array, moving all the elements
// in the array to make space for a new element at the start of the array. The
// failure modes and caveats for `ds_array_ins_tail()` around returning NULL
// and errors apply to `ds_array_ins_head()` as well.
void *ds_array_ins_head (ds_array_t *ll, void *el);

// Removes the final element from the end of the array, returning it. The array
// length is decremented. Note that NULL will be returned if that is the value
// of the pointer at the end of the array. Returns NULL if `ll` is NULL.
void *ds_array_rm_tail (ds_array_t *ll);

// Removes the first element of the array, returning it. The array length is
// decremented. Note that NULL will be returned if that is the value of the
// pointer at first position in the array. Returns NULL if `ll` is NULL.
void *ds_array_rm_head (ds_array_t *ll);

// Removes the element at the specified position in the array, returning it.
// On OOB error a NULL is returned. Note that NULL can be a successful return
// value if the element at position `index` is a NULL pointer, in which case
// the caller must use `ds_array_length()` to determine that the removal
// succeeded. Returns NULL if `ll` is NULL or `index` is OOB.
void *ds_array_rm (ds_array_t *ll, size_t index);

// Searches for and removes the first occurrence of the specified pointer from
// the array. Returns NULL on error or if `ll` is NULL.
void *ds_array_rm_ptr (ds_array_t *ll, const void *ptr);

// When elements are removed from an array, the count of elements in the array
// is decremented by the backing memory but not reallocated to a smaller block.
// This function reallocates the backing memory to exactly fit the number of
// elements in the array.
void ds_array_shrink_to_fit (ds_array_t *ll);

// Find the position of the specified pointer in the specified array,
// returning `(size_t)-1` if `ptr` is not found. As the array may contain
// duplicate pointers, only the position of the first matching pointer is
// returned. If `ll` is NULL `(size_t)-1` is returned.
// As the array may store NULL, searching for the position of the first NULL
// pointer is valid, and returns the position of the first NULL pointer.
size_t ds_array_ptr_index (const ds_array_t *ll, const void *ptr);

// Returns all elements in the array as a single C array of void pointers. The
// number of elements is stored in `dstlen`, the freshly allocated C array is
// stored in `dst` and is returned to the caller.
// On error NULL is returned. If `ll` is NULL or if the number of elements in
// the array is `0`, NULL is returned.
// The returned C array must be freed by the caller. Note that the elements of
// the C array would still be stored within the `ll` array.
void **ds_array_all (ds_array_t *ll, void ***dst, size_t *dstlen);

#ifdef __cplusplus
};
#endif

#endif


