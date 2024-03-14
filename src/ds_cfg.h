
#ifndef H_DS_CFG
#define H_DS_CFG

#include <stdbool.h>
#include <stdlib.h>

typedef struct ds_cfg_t ds_cfg_t;

struct ds_cfg_record_t {
   char *section;
   char *key;
   char *value;
};


#ifdef __cplusplus
extern "C" {
#endif

   // Create and delete configuration objects. Configuration objects are
   // simply a tuple of {section, key, value}. Duplicates are stored. The caller
   // is responsible for detecting and ignoring duplicates.
   ds_cfg_t *ds_cfg_new (void);

   // Loads configuration from the specified file. Any errors that are encountered
   // are recored in errcode and errmsg, if they are non-NULL. The number of lines
   // processed will be recorded in `nlines` when `nlines` is non-NULL. Processing
   // stops at the first error encountered, so `nlines` will always contain the line
   // number of the error.
   //
   // When no errors are encountered, `nlines` will contain the number of lines
   // in the file.
   //
   // The `nlines` parameter is optional and will be ignored if NULL.
   // The `errcode` parameter is optional and will be ignored if NULL.
   // The `errmsg` parameter is optional and will be ignored if NULL.
   //
   ds_cfg_t *ds_cfg_load (const char *fname, size_t max_line_len, size_t *nlines,
                                 int *errcode, char **errmsg);
   void ds_cfg_del (ds_cfg_t *cfg);

   // Set a single value for a specific key in the specified section. If section
   // is NULL or empty, the default section is used. Copies of the key and the
   // values are made, so the caller need not keep them after this call returns.
   bool ds_cfg_add (ds_cfg_t *cfg,
                    const char *sect, const char *k, const char *v);

   // Removes the specified value from the specified section. Returns nothing.
   void ds_cfg_rm (ds_cfg_t *cfg, const char *sect, const char *k);

   // Returns a pointer to the value for key `k` in section `sect`, or NULL if
   // the key is not found in the specified section.
   //
   // Use a section of "" for the default section.
   const char *ds_cfg_get (ds_cfg_t *cfg, const char *sect, const char *k);

   // Returns the number of configuration tuples stored.
   size_t ds_cfg_count (const ds_cfg_t *cfg);

   // Retrieves all the configuration values in a single array of ds_cfg_record_t
   // objects. The caller:
   // 1. MUST NOT free the returned array from this function,
   // 2. MUST NOT free the elements of the array,
   // 3. MUST NOT freen each field of each element within the array.
   // 4. MUST NOT use the returned value after the corresponding `ds_cfg_t`
   //    object has been deleted.
   //
   // In brief, the returned array is a field of the ds_cfg_t object, and must
   // only ever be read while the corresponding ds_cfg_t object is valid. If the
   // corresponding ds_cfg_t object is changed via ds_cfg_add() or ds_cfg_rm(), this
   // array becomes invalid.
   struct ds_cfg_record_t *ds_cfg_all (ds_cfg_t *cfg);

#ifdef __cplusplus
};
#endif


#endif


