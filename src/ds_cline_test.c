#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ds_kvpair.h"
#include "ds_cline.h"
#include "ds_str.h"


static bool test_args (const char *name, int argc, char **argv)
{
  ds_cline_t *cline = NULL;

  if (!(cline = ds_cline_parse (argc, argv))) {
    fprintf (stderr, "Failed to parse [%s]\n", name);
    return false;
  }
  size_t nflags = ds_cline_flag_count (cline);
  size_t nargs = ds_cline_arg_count (cline);

  printf ("[%s]: %zu args, %zu flags\n", name, nargs, nflags);

  for (size_t i=0; i<=nflags; i++) {
    const ds_kvpair_t *flag = ds_cline_flag_get (cline, i);
    printf ("[%s]: flag [%zu]: [%s] = [%s]\n",
            name,
            i,
            flag ? ds_kvpair_key_get (flag) : "(null-key)",
            flag ? ds_kvpair_value_get (flag) : "(null-value)");
  }

  for (size_t i=0; i<=nargs; i++) {
    const char *arg = ds_cline_arg_get (cline, i);
    printf ("[%s]: arg [%zu]: [%s]\n", name, i, arg ? arg : "(null-arg)");
  }

  for (int i=0; i < argc; i++) {
    free (argv[i]);
  }
  ds_cline_del (cline);
  return true;
}

int main (void)
{
  int ret = EXIT_FAILURE;

  int argc1 = 1;
  char *argv1[] = { "prog", NULL };

  int argc2 = 4;
  char *argv2[] = { "prog", "arg", "--flag1=two", "arg2", NULL };

  int argc3 = 5;
  char *argv3[] = {
    "prog", "--flag 1=two values", "--no-value", "--", "extra-arg", NULL
  };


  struct {
    const char *name;
    int argc;
    char **argv;
  } tests[] = {
    { "No args",              argc1, argv1 },
    { "arg, flag, arg",       argc2, argv2 },
    { "no-value, no-key",     argc3, argv3 },
  };
  static const size_t ntests = sizeof tests / sizeof tests[0];

  bool rc = true;
  for (size_t i=0; i<ntests; i++) {
    // Set all the strings to writable - test function will have to free them
    for (size_t j=0; tests[i].argv[j]; j++) {
      if (!(tests[i].argv[j] = ds_str_dup (tests[i].argv[j]))) {
        printf ("OOM error reallocating strings\n");
        return EXIT_FAILURE;
      }
    }
    bool passed = test_args (tests[i].name, tests[i].argc, tests[i].argv);
    rc = rc && passed;
    const char *msg = passed ? "passed" : "failed";
    printf ("Test [%s]: %s\n", tests[i].name, msg);
  }
  ret = rc ? EXIT_SUCCESS : EXIT_FAILURE;
  return ret;
}



