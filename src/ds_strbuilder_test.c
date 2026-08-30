#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ds_strbuilder.h"

int main (void)
{
  int ret = EXIT_FAILURE;

  static const char *test_strings[] = {
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "Seven",
    "Eight",
    "Nine",
    "Ten",
  };
  char *str = NULL;

  ds_strbuilder_t *sb = ds_strbuilder_new (NULL);
  if (!sb) {
    fprintf (stderr, "Failed to create string builder\n");
    goto cleanup;
  }

  for (size_t i=0; i<(sizeof test_strings / sizeof test_strings[0]) - 1; i++) {
    if (!ds_strbuilder_append (sb, test_strings[i], test_strings[i+1], NULL)) {
      fprintf (stderr, "Failed to append string [%zu]: %s\n", i, test_strings[i]);
      goto cleanup;
    }
  }

  if (!(str = ds_strbuilder_build (sb))) {
    fprintf (stderr, "Failed to build string\n");
    goto cleanup;
  }

  printf ("[%s]\n", str);

  ret = EXIT_SUCCESS;
cleanup:
  free (str);
  ds_strbuilder_del (sb);
  return ret;
}



