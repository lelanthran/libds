
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


  // Retrive the args. The caller must not free the returned value.
  const char *ds_cline_arg_get (ds_cline_t *cline, size_t i);

  // Retrieve the flags; the caller must not delete the returned value.
  // Modification is not allowed, even using the ds_kvpair_* functions.
  const ds_kvpair_t *ds_cline_flag_get (ds_cline_t *cline, size_t i);


#ifdef __cplusplus
};
#endif


#endif


