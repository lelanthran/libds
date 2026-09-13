#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ds_array.h"

#define LOG_MSG(...)       do {\
  printf ("[%s:%i] ", __FILE__, __LINE__);\
  printf (__VA_ARGS__);\
} while (0)

#define FAILED(n)     printf ("FAILED:     [%s]", n)
#define PASSED(n)     printf ("PASSED:     [%s]", n)


struct collect_context {
  int *result;
  size_t count;
};


static void collect (void *el, void *param)
{
  struct collect_context *context = param;

  context->result[context->count++] = *(int *) el;
}


static void count_element (void *el, void *param)
{
  size_t *count = param;

  (void) el;
  (*count)++;
}


static bool test_new (void)
{
  bool error = true;
  ds_array_t *a = ds_array_new ();

  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 0) {
    LOG_MSG ("New array does not have length zero\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_del (void)
{
  bool error = true;
  ds_array_t *a = ds_array_new ();

  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  ds_array_del (a);
  a = NULL;

  ds_array_del (NULL);

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_length (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_length (NULL) != 0) {
    LOG_MSG ("NULL array did not return zero length\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 0) {
    LOG_MSG ("Empty array does not have length zero\n");
    goto cleanup;
  }

  if (!ds_array_ins_tail (a, &values[0])) {
    LOG_MSG ("Failed to insert first value\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1) {
    LOG_MSG ("Length is not one after one insertion\n");
    goto cleanup;
  }

  if (!ds_array_ins_tail (a, &values[1])) {
    LOG_MSG ("Failed to insert second value\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to insert third value\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 3) {
    LOG_MSG ("Length is not three after three insertions\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_get (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_get (NULL, 0) != NULL) {
    LOG_MSG ("get(NULL, 0) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != NULL) {
    LOG_MSG ("get() on empty array did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0]) {
    LOG_MSG ("Failed to insert first value\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[1]) != &values[1]) {
    LOG_MSG ("Failed to insert second value\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to insert third value\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &values[0]) {
    LOG_MSG ("get(0) returned incorrect pointer\n");
    goto cleanup;
  }

  if (ds_array_get (a, 1) != &values[1]) {
    LOG_MSG ("get(1) returned incorrect pointer\n");
    goto cleanup;
  }

  if (ds_array_get (a, 2) != &values[2]) {
    LOG_MSG ("get(2) returned incorrect pointer\n");
    goto cleanup;
  }

  if (ds_array_get (a, 3) != NULL) {
    LOG_MSG ("get() at length did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_get (a, (size_t) -1) != NULL) {
    LOG_MSG ("get() with SIZE_MAX did not return NULL\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_ins_tail (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_ins_tail (NULL, &values[0]) != NULL) {
    LOG_MSG ("ins_tail(NULL, element) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, NULL) != NULL) {
    LOG_MSG ("ins_tail(array, NULL) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1) {
    LOG_MSG ("unexpected array length\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0]) {
    LOG_MSG ("First tail insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[1]) != &values[1]) {
    LOG_MSG ("Second tail insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Third tail insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 4) {
    LOG_MSG ("Tail insertion produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != NULL ||
      ds_array_get (a, 1) != &values[0] ||
      ds_array_get (a, 2) != &values[1] ||
      ds_array_get (a, 3) != &values[2]) {
    LOG_MSG ("Tail insertion produced incorrect ordering\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_ins_head (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_ins_head (NULL, &values[0]) != NULL) {
    LOG_MSG ("ins_head(NULL, element) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, NULL) != NULL) {
    LOG_MSG ("ins_head(array, NULL) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1) {
    LOG_MSG ("unexpected array length\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, &values[0]) != &values[0]) {
    LOG_MSG ("First head insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, &values[1]) != &values[1]) {
    LOG_MSG ("Second head insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, &values[2]) != &values[2]) {
    LOG_MSG ("Third head insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 4) {
    LOG_MSG ("Head insertion produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &values[2] ||
      ds_array_get (a, 1) != &values[1] ||
      ds_array_get (a, 2) != &values[0] ||
      ds_array_get (a, 3) != NULL) {
    LOG_MSG ("Head insertion produced incorrect ordering\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_null_element (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int value = 42;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, NULL) != NULL) {
    LOG_MSG ("NULL insertion unexpectedly returned non-NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1) {
    LOG_MSG ("NULL element was not inserted\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != NULL) {
    LOG_MSG ("Stored NULL element was not returned as NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, &value) != &value) {
    LOG_MSG ("Failed to insert non-NULL element before NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2 ||
      ds_array_get (a, 0) != &value ||
      ds_array_get (a, 1) != NULL) {
    LOG_MSG ("NULL element was not retained after insertion\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm_tail (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_rm_tail (NULL) != NULL) {
    LOG_MSG ("rm_tail(NULL) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_rm_tail (a) != NULL) {
    LOG_MSG ("rm_tail(empty) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_rm_tail (a) != &values[2]) {
    LOG_MSG ("rm_tail() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2 ||
      ds_array_get (a, 0) != &values[0] ||
      ds_array_get (a, 1) != &values[1]) {
    LOG_MSG ("Array incorrect after first rm_tail()\n");
    goto cleanup;
  }

  if (ds_array_rm_tail (a) != &values[1]) {
    LOG_MSG ("Second rm_tail() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_rm_tail (a) != &values[0]) {
    LOG_MSG ("Third rm_tail() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 0) {
    LOG_MSG ("Array did not become empty\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm_head (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[3] = { 1, 2, 3 };

  if (ds_array_rm_head (NULL) != NULL) {
    LOG_MSG ("rm_head(NULL) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_rm_head (a) != NULL) {
    LOG_MSG ("rm_head(empty) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_rm_head (a) != &values[0]) {
    LOG_MSG ("rm_head() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2 ||
      ds_array_get (a, 0) != &values[1] ||
      ds_array_get (a, 1) != &values[2]) {
    LOG_MSG ("Array incorrect after first rm_head()\n");
    goto cleanup;
  }

  if (ds_array_rm_head (a) != &values[1]) {
    LOG_MSG ("Second rm_head() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_rm_head (a) != &values[2]) {
    LOG_MSG ("Third rm_head() returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 0) {
    LOG_MSG ("Array did not become empty\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[5] = { 1, 2, 3, 4, 5 };
  size_t old_length;

  if (ds_array_rm (NULL, 0) != NULL) {
    LOG_MSG ("rm(NULL, 0) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_rm (a, 0) != NULL) {
    LOG_MSG ("rm(empty, 0) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[3]) != &values[3] ||
      ds_array_ins_tail (a, &values[4]) != &values[4]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  old_length = ds_array_length (a);

  if (ds_array_rm (a, 0) != &values[0]) {
    LOG_MSG ("rm(0) returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != old_length - 1 ||
      ds_array_get (a, 0) != &values[1] ||
      ds_array_get (a, 1) != &values[2] ||
      ds_array_get (a, 2) != &values[3] ||
      ds_array_get (a, 3) != &values[4]) {
    LOG_MSG ("rm(0) did not shift elements correctly\n");
    goto cleanup;
  }

  if (ds_array_rm (a, 2) != &values[3]) {
    LOG_MSG ("rm(middle) returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 3 ||
      ds_array_get (a, 0) != &values[1] ||
      ds_array_get (a, 1) != &values[2] ||
      ds_array_get (a, 2) != &values[4]) {
    LOG_MSG ("rm(middle) did not shift elements correctly\n");
    goto cleanup;
  }

  if (ds_array_rm (a, 2) != &values[4]) {
    LOG_MSG ("rm(last) returned wrong element\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2) {
    LOG_MSG ("rm(last) produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_rm (a, ds_array_length (a)) != NULL) {
    LOG_MSG ("rm(length) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_rm (a, (size_t) -1) != NULL) {
    LOG_MSG ("rm(SIZE_MAX) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2) {
    LOG_MSG ("OOB removal changed array length\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm_null (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[2] = { 1, 2 };

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, NULL) != NULL ||
      ds_array_ins_tail (a, &values[1]) != &values[1]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 3) {
    LOG_MSG ("Unexpected initial length\n");
    goto cleanup;
  }

  if (ds_array_rm (a, 1) != NULL) {
    LOG_MSG ("rm() of NULL element returned non-NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2 ||
      ds_array_get (a, 0) != &values[0] ||
      ds_array_get (a, 1) != &values[1]) {
    LOG_MSG ("rm() did not remove the NULL element correctly\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm_ptr (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[5] = { 1, 2, 3, 4, 5 };
  int absent = 99;

  if (ds_array_rm_ptr (NULL, &values[0]) != NULL) {
    LOG_MSG ("rm_ptr(NULL, ptr) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &absent) != NULL) {
    LOG_MSG ("rm_ptr() found an absent pointer\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[3]) != &values[3] ||
      ds_array_ins_tail (a, &values[4]) != &values[4]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &values[2]) != &values[2]) {
    LOG_MSG ("rm_ptr() returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 4 ||
      ds_array_get (a, 0) != &values[0] ||
      ds_array_get (a, 1) != &values[1] ||
      ds_array_get (a, 2) != &values[3] ||
      ds_array_get (a, 3) != &values[4]) {
    LOG_MSG ("rm_ptr() did not remove and shift correctly\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &absent) != NULL) {
    LOG_MSG ("rm_ptr() found an absent pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 4) {
    LOG_MSG ("Failed rm_ptr() changed length\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &values[0]) != &values[0]) {
    LOG_MSG ("rm_ptr() failed to remove first element\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &values[4]) != &values[4]) {
    LOG_MSG ("rm_ptr() failed to remove last element\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_rm_ptr_null (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int value = 42;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &value) != &value ||
      ds_array_ins_tail (a, NULL) != NULL ||
      ds_array_ins_tail (a, &value) != &value) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, NULL) != NULL) {
    LOG_MSG ("rm_ptr(NULL) returned non-NULL\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 2) {
    LOG_MSG ("rm_ptr(NULL) did not remove the NULL element\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &value ||
      ds_array_get (a, 1) != &value) {
    LOG_MSG ("rm_ptr(NULL) changed the wrong elements\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_remove_pointer_identity (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[2] = { 10, 10 };

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &values[1]) != &values[1]) {
    LOG_MSG ("rm_ptr() did not use pointer identity\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1 ||
      ds_array_get (a, 0) != &values[0]) {
    LOG_MSG ("rm_ptr() removed the wrong equal-valued object\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_iterate (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[4] = { 10, 20, 30, 40 };
  int result[4] = { 0 };
  struct collect_context context = {
    .result = result,
    .count = 0
  };

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[3]) != &values[3]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  ds_array_iterate (a, collect, &context);

  if (context.count != 4 ||
      result[0] != 10 ||
      result[1] != 20 ||
      result[2] != 30 ||
      result[3] != 40) {
    LOG_MSG ("iterate() did not visit elements in forward order\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_iterate_reverse (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[4] = { 10, 20, 30, 40 };
  int result[4] = { 0 };
  struct collect_context context = {
    .result = result,
    .count = 0
  };

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[3]) != &values[3]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  ds_array_iterate_reverse (a, collect, &context);

  if (context.count != 4 ||
      result[0] != 40 ||
      result[1] != 30 ||
      result[2] != 20 ||
      result[3] != 10) {
    LOG_MSG ("iterate_reverse() did not visit elements in reverse order\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_iterate_empty (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  size_t count = 0;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  ds_array_iterate (a, count_element, &count);
  ds_array_iterate_reverse (a, count_element, &count);

  if (count != 0) {
    LOG_MSG ("Iteration invoked callback on empty array\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_copy (void)
{
  bool error = true;
  ds_array_t *src = NULL;
  ds_array_t *dst = NULL;
  int values[5] = { 10, 20, 30, 40, 50 };

  src = ds_array_new ();
  if (!src) {
    LOG_MSG ("Failed to create source array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (src, &values[0]) != &values[0] ||
      ds_array_ins_tail (src, &values[1]) != &values[1] ||
      ds_array_ins_tail (src, &values[2]) != &values[2] ||
      ds_array_ins_tail (src, &values[3]) != &values[3] ||
      ds_array_ins_tail (src, &values[4]) != &values[4]) {
    LOG_MSG ("Failed to populate source array\n");
    goto cleanup;
  }

  dst = ds_array_copy (src, 1, 4);
  if (!dst) {
    LOG_MSG ("Failed to copy array range\n");
    goto cleanup;
  }

  if (ds_array_length (dst) != 3) {
    LOG_MSG ("Copied array has incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (dst, 0) != &values[1] ||
      ds_array_get (dst, 1) != &values[2] ||
      ds_array_get (dst, 2) != &values[3]) {
    LOG_MSG ("Copied array contains incorrect pointers\n");
    goto cleanup;
  }

  values[2] = 99;

  if (*(int *) ds_array_get (src, 2) != 99 ||
      *(int *) ds_array_get (dst, 1) != 99) {
    LOG_MSG ("Copy does not share elements with source\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (dst);
  ds_array_del (src);
  return !error;
}


static bool test_copy_full (void)
{
  bool error = true;
  ds_array_t *src = NULL;
  ds_array_t *dst = NULL;
  int values[3] = { 10, 20, 30 };

  src = ds_array_new ();
  if (!src) {
    LOG_MSG ("Failed to create source array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (src, &values[0]) != &values[0] ||
      ds_array_ins_tail (src, &values[1]) != &values[1] ||
      ds_array_ins_tail (src, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to populate source array\n");
    goto cleanup;
  }

  dst = ds_array_copy (src, 0, 3);
  if (!dst) {
    LOG_MSG ("Failed to copy complete array\n");
    goto cleanup;
  }

  if (ds_array_length (dst) != 3 ||
      ds_array_get (dst, 0) != &values[0] ||
      ds_array_get (dst, 1) != &values[1] ||
      ds_array_get (dst, 2) != &values[2]) {
    LOG_MSG ("Full copy is incorrect\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (dst);
  ds_array_del (src);
  return !error;
}


static bool test_copy_bounds (void)
{
  bool error = true;
  ds_array_t *src = NULL;
  ds_array_t *dst = NULL;
  int values[3] = { 10, 20, 30 };

  if (ds_array_copy (NULL, 0, 1) != NULL) {
    LOG_MSG ("copy(NULL) did not return NULL\n");
    goto cleanup;
  }

  src = ds_array_new ();
  if (!src) {
    LOG_MSG ("Failed to create source array\n");
    goto cleanup;
  }

  if (ds_array_copy (src, 0, 0) != NULL) {
    LOG_MSG ("copy() of empty range did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (src, &values[0]) != &values[0] ||
      ds_array_ins_tail (src, &values[1]) != &values[1] ||
      ds_array_ins_tail (src, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to populate source array\n");
    goto cleanup;
  }

  dst = ds_array_copy (src, 2, 1);
  ds_array_del (dst);
  dst = NULL;

  dst = ds_array_copy (src, 0, 4);
  ds_array_del (dst);
  dst = NULL;

  dst = ds_array_copy (src, 4, 4);
  ds_array_del (dst);
  dst = NULL;

  error = false;
cleanup:
  ds_array_del (dst);
  ds_array_del (src);
  return !error;
}


static bool test_shrink_to_fit (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

  ds_array_shrink_to_fit (NULL);

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  ds_array_shrink_to_fit (a);

  if (ds_array_length (a) != 0) {
    LOG_MSG ("Shrink changed empty array length\n");
    goto cleanup;
  }

  for (size_t i = 0; i < 8; i++) {
    if (ds_array_ins_tail (a, &values[i]) != &values[i]) {
      LOG_MSG ("Failed to populate array\n");
      goto cleanup;
    }
  }

  if (ds_array_rm (a, 4) != &values[4] ||
      ds_array_rm (a, 2) != &values[2]) {
    LOG_MSG ("Failed to remove elements before shrink\n");
    goto cleanup;
  }

  ds_array_shrink_to_fit (a);

  if (ds_array_length (a) != 6) {
    LOG_MSG ("Shrink changed array length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &values[0] ||
      ds_array_get (a, 1) != &values[1] ||
      ds_array_get (a, 2) != &values[3] ||
      ds_array_get (a, 3) != &values[5] ||
      ds_array_get (a, 4) != &values[6] ||
      ds_array_get (a, 5) != &values[7]) {
    LOG_MSG ("Shrink changed array contents\n");
    goto cleanup;
  }

  if (ds_array_ins_head (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[6]) != &values[6]) {
    LOG_MSG ("Array cannot be used after shrink\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 8) {
    LOG_MSG ("Incorrect length after post-shrink insertions\n");
    goto cleanup;
  }

  error = false;
cleanup:
  ds_array_del (a);
  return !error;
}


static bool test_all (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  void **dst = NULL;
  size_t dstlen = 0;
  int values[3] = { 10, 20, 30 };

  if (ds_array_all (NULL, &dst, &dstlen) != NULL) {
    LOG_MSG ("all(NULL) did not return NULL\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_all (a, &dst, &dstlen) != NULL) {
    LOG_MSG ("all(empty) did not return NULL\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  dst = NULL;
  dstlen = 0;

  if (!ds_array_all (a, &dst, &dstlen)) {
    LOG_MSG ("all() failed\n");
    goto cleanup;
  }

  if (dstlen != 3) {
    LOG_MSG ("all() returned incorrect length\n");
    goto cleanup;
  }

  if (dst[0] != &values[0] ||
      dst[1] != &values[1] ||
      dst[2] != &values[2]) {
    LOG_MSG ("all() returned incorrect pointers\n");
    goto cleanup;
  }

  dst[0] = &values[2];

  if (ds_array_get (a, 0) != &values[0]) {
    LOG_MSG ("all() returned the backing array itself\n");
    goto cleanup;
  }

  error = false;
cleanup:
  free (dst);
  ds_array_del (a);
  return !error;
}


static bool test_all_null_element (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  void **dst = NULL;
  size_t dstlen = 0;
  int value = 42;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, NULL) != NULL ||
      ds_array_ins_tail (a, &value) != &value) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  if (!ds_array_all (a, &dst, &dstlen)) {
    LOG_MSG ("all() failed with NULL element\n");
    goto cleanup;
  }

  if (dstlen != 2 ||
      dst[0] != NULL ||
      dst[1] != &value) {
    LOG_MSG ("all() did not preserve NULL element\n");
    goto cleanup;
  }

  error = false;
cleanup:
  free (dst);
  ds_array_del (a);
  return !error;
}


static bool test_combined_operations (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[6] = { 1, 2, 3, 4, 5, 6 };
  void **all = NULL;
  size_t all_length = 0;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_head (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_head (a, &values[3]) != &values[3]) {
    LOG_MSG ("Failed during combined insertion test\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 4 ||
      ds_array_get (a, 0) != &values[3] ||
      ds_array_get (a, 1) != &values[0] ||
      ds_array_get (a, 2) != &values[1] ||
      ds_array_get (a, 3) != &values[2]) {
    LOG_MSG ("Combined insertion operations produced wrong array\n");
    goto cleanup;
  }

  if (ds_array_rm_ptr (a, &values[0]) != &values[0]) {
    LOG_MSG ("Combined rm_ptr() failed\n");
    goto cleanup;
  }

  if (ds_array_rm_head (a) != &values[3]) {
    LOG_MSG ("Combined rm_head() failed\n");
    goto cleanup;
  }

  if (ds_array_rm_tail (a) != &values[2]) {
    LOG_MSG ("Combined rm_tail() failed\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 1 ||
      ds_array_get (a, 0) != &values[1]) {
    LOG_MSG ("Combined removals produced wrong array\n");
    goto cleanup;
  }

  if (!ds_array_all (a, &all, &all_length)) {
    LOG_MSG ("all() failed after combined operations\n");
    goto cleanup;
  }

  if (all_length != 1 || all[0] != &values[1]) {
    LOG_MSG ("all() returned wrong combined result\n");
    goto cleanup;
  }

  free (all);
  all = NULL;

  ds_array_shrink_to_fit (a);

  if (ds_array_length (a) != 1 ||
      ds_array_get (a, 0) != &values[1]) {
    LOG_MSG ("shrink_to_fit() corrupted combined result\n");
    goto cleanup;
  }

  error = false;
cleanup:
  free (all);
  ds_array_del (a);
  return !error;
}


static bool test_ins (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[] = {10, 20, 30, 40};
  int inserted = 25;
  void *result = NULL;

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  for (size_t i = 0; i < 4; i++) {
    if (!ds_array_ins_tail (a, &values[i])) {
      LOG_MSG ("Failed to populate array with initial values\n");
      goto cleanup;
    }
  }

  /*
   * Insert into the middle:
   *
   *   10 20 30 40
   *         ^
   *         insert 25 at position 2
   *
   *   10 20 25 30 40
   */
  result = ds_array_ins (a, &inserted, 2);
  if (result != &inserted) {
    LOG_MSG ("Middle insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 5) {
    LOG_MSG ("Middle insertion produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &values[0]) {
    LOG_MSG ("Middle insertion corrupted element at position 0\n");
    goto cleanup;
  }

  if (ds_array_get (a, 1) != &values[1]) {
    LOG_MSG ("Middle insertion corrupted element at position 1\n");
    goto cleanup;
  }

  if (ds_array_get (a, 2) != &inserted) {
    LOG_MSG ("Middle insertion did not place new element at position 2\n");
    goto cleanup;
  }

  if (ds_array_get (a, 3) != &values[2]) {
    LOG_MSG ("Middle insertion did not shift element to position 3\n");
    goto cleanup;
  }

  if (ds_array_get (a, 4) != &values[3]) {
    LOG_MSG ("Middle insertion did not shift element to position 4\n");
    goto cleanup;
  }

  /*
   * Insert at the head. This re-inserts the pointer already stored at
   * position 0 (values[0]) to confirm that duplicate pointers are
   * allowed: the existing occurrence is not deduplicated, it is simply
   * shifted along with everything else.
   *
   *   10 20 25 30 40
   *   ^
   *   insert 10 (values[0]) at position 0
   *
   *   10 10 20 25 30 40
   */
  result = ds_array_ins (a, &values[0], 0);
  if (result != &values[0]) {
    LOG_MSG ("Head insertion returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 6) {
    LOG_MSG ("Head insertion produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 0) != &values[0]) {
    LOG_MSG ("Head insertion did not place new element at position 0\n");
    goto cleanup;
  }

  if (ds_array_get (a, 1) != &values[0]) {
    LOG_MSG ("Head insertion did not shift original position-0 element to position 1\n");
    goto cleanup;
  }

  if (ds_array_get (a, 2) != &values[1]) {
    LOG_MSG ("Head insertion did not shift element to position 2\n");
    goto cleanup;
  }

  if (ds_array_get (a, 3) != &inserted) {
    LOG_MSG ("Head insertion did not shift element to position 3\n");
    goto cleanup;
  }

  if (ds_array_get (a, 4) != &values[2]) {
    LOG_MSG ("Head insertion did not shift element to position 4\n");
    goto cleanup;
  }

  if (ds_array_get (a, 5) != &values[3]) {
    LOG_MSG ("Head insertion did not shift element to position 5\n");
    goto cleanup;
  }

  /*
   * Insert at the tail. Position == length should append.
   *
   *   10 10 20 25 30 40
   *                     ^
   *                     insert 30 (values[2]) at position 6 (== length)
   *
   *   10 10 20 25 30 40 30
   */
  result = ds_array_ins (a, &values[2], ds_array_length (a));
  if (result != &values[2]) {
    LOG_MSG ("Tail insertion (pos == length) returned wrong pointer\n");
    goto cleanup;
  }

  if (ds_array_length (a) != 7) {
    LOG_MSG ("Tail insertion (pos == length) produced incorrect length\n");
    goto cleanup;
  }

  if (ds_array_get (a, 6) != &values[2]) {
    LOG_MSG ("Tail insertion (pos == length) did not append element\n");
    goto cleanup;
  }

  error = false;

cleanup:
  ds_array_del (a);
  return !error;
}

static bool test_ptr_index (void)
{
  bool error = true;
  ds_array_t *a = NULL;
  int values[5] = { 10, 20, 30, 40, 50 };
  int absent = 99;
  int duplicate = 10;   /* equal in value to values[0], distinct object */

  /* NULL array must report not-found. */
  if (ds_array_ptr_index (NULL, &values[0]) != (size_t) -1) {
    LOG_MSG ("ptr_index(NULL, ptr) did not return (size_t)-1\n");
    goto cleanup;
  }

  a = ds_array_new ();
  if (!a) {
    LOG_MSG ("Failed to create new array\n");
    goto cleanup;
  }

  /* Empty array must report not-found. */
  if (ds_array_ptr_index (a, &values[0]) != (size_t) -1) {
    LOG_MSG ("ptr_index(empty, ptr) did not return (size_t)-1\n");
    goto cleanup;
  }

  /* Searching for NULL in an empty array must report not-found. */
  if (ds_array_ptr_index (a, NULL) != (size_t) -1) {
    LOG_MSG ("ptr_index(empty, NULL) did not return (size_t)-1\n");
    goto cleanup;
  }

  if (ds_array_ins_tail (a, &values[0]) != &values[0] ||
      ds_array_ins_tail (a, &values[1]) != &values[1] ||
      ds_array_ins_tail (a, &values[2]) != &values[2] ||
      ds_array_ins_tail (a, &values[3]) != &values[3] ||
      ds_array_ins_tail (a, &values[4]) != &values[4]) {
    LOG_MSG ("Failed to populate array\n");
    goto cleanup;
  }

  /* Each stored pointer must be found at its own index. */
  if (ds_array_ptr_index (a, &values[0]) != 0) {
    LOG_MSG ("ptr_index() failed to find first element\n");
    goto cleanup;
  }

  if (ds_array_ptr_index (a, &values[2]) != 2) {
    LOG_MSG ("ptr_index() failed to find middle element\n");
    goto cleanup;
  }

  if (ds_array_ptr_index (a, &values[4]) != 4) {
    LOG_MSG ("ptr_index() failed to find last element\n");
    goto cleanup;
  }

  /* Absent pointer must report not-found. */
  if (ds_array_ptr_index (a, &absent) != (size_t) -1) {
    LOG_MSG ("ptr_index() found an absent pointer\n");
    goto cleanup;
  }

  /*
   * Pointer identity: `duplicate` has the same value as `values[0]`
   * but is a distinct object. It must NOT be found.
   */
  if (ds_array_ptr_index (a, &duplicate) != (size_t) -1) {
    LOG_MSG ("ptr_index() matched on value instead of pointer identity\n");
    goto cleanup;
  }

  /*
   * Duplicate pointers: insert the same pointer twice and ensure the
   * first occurrence is returned.
   *
   *   10 20 30 40 50 10
   *                   ^ values[0] again
   *
   * The first occurrence of &values[0] is still index 0.
   */
  if (ds_array_ins_tail (a, &values[0]) != &values[0]) {
    LOG_MSG ("Failed to insert duplicate pointer\n");
    goto cleanup;
  }

  if (ds_array_ptr_index (a, &values[0]) != 0) {
    LOG_MSG ("ptr_index() did not return first occurrence of duplicate\n");
    goto cleanup;
  }

  /* Searching for NULL when none is stored must report not-found. */
  if (ds_array_ptr_index (a, NULL) != (size_t) -1) {
    LOG_MSG ("ptr_index() found NULL when none was stored\n");
    goto cleanup;
  }

  error = false;

cleanup:
  ds_array_del (a);
  return !error;
}

int main (void)
{
  int ret = 0;
#define TEST(x)   { x, #x }
  static const struct {
    bool (*fptr) (void);
    const char *name;
  } tests[] = {
    TEST (test_new),
    TEST (test_del),
    TEST (test_length),
    TEST (test_get),
    TEST (test_ins_tail),
    TEST (test_ins_head),
    TEST (test_null_element),
    TEST (test_rm_tail),
    TEST (test_rm_head),
    TEST (test_rm),
    TEST (test_rm_null),
    TEST (test_rm_ptr),
    TEST (test_rm_ptr_null),
    TEST (test_remove_pointer_identity),
    TEST (test_iterate),
    TEST (test_iterate_reverse),
    TEST (test_iterate_empty),
    TEST (test_copy),
    TEST (test_copy_full),
    TEST (test_copy_bounds),
    TEST (test_shrink_to_fit),
    TEST (test_all),
    TEST (test_all_null_element),
    TEST (test_combined_operations),
    TEST (test_ins),
    TEST (test_ptr_index),
  };
#undef TEST
  static const size_t ntests = sizeof tests / sizeof tests[0];

  for (size_t i = 0; i < ntests; i++) {
    bool passed = tests[i].fptr ();

    if (!passed) {
      ret++;
      FAILED (tests[i].name);
      printf ("\n");
    } else {
      PASSED (tests[i].name);
      printf ("\n");
    }
  }

  return ret;
}
