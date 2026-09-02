
#ifndef H_DS_CLINE
#define H_DS_CLINE


typedef struct ds_cline_t ds_cline_t;

#ifdef __cplusplus
extern "C" {
#endif

  // This is a destructive parse of the argument parameters. Once this
  // function returns argv will have elements in an indeterminate state and
  // thus should not be used.
  ds_cline_t *ds_cline_parse (int argc, char **argv);
  void ds_cline_del (ds_cline_t *cline);


  // Get a count of the args and flags
  size_t ds_cline_arg_count (const ds_cline_t *cline);
  size_t ds_cline_flag_count (const ds_cline_t *cline);


  // Retrieve the args. The caller must not free the returned value.
  const char *ds_cline_arg_get (ds_cline_t *cline, size_t i);

  // Retrieve the flags; the caller must not delete the returned value.
  // Modification is not allowed, even using the ds_kvpair_* functions. If the
  // specified flag does not exist, NULL is returned.
  const ds_kvpair_t *ds_cline_flag_get (ds_cline_t *cline, size_t i);

  // Convenience function to determine if arguments are present. Useful when
  // a `verbose` or `help` command is necessary.
  bool ds_cline_arg_test (const ds_cline_t *cline, const char *arg);

  // Another convenience function: return the value of a specific flag. Caller
  // may not modify or free the returned value. Returns only the first one
  // encountered (there mya be multiple as duplicates are allowed.
  const char *ds_cline_flag_value (const ds_cline_t *cline, const char *flag);

  // Yet another convenience function, to return all the values that match a
  // specific flag (duplicate flags are allowed). Caller must free the
  // returned array, but not free any of the elements in the returned array.
  // The returned array is terminated with a NULL pointer.
  const char **ds_cline_flag_values (ds_cline_t *cline, const char *flag);


#ifdef __cplusplus
};
#endif


#endif


