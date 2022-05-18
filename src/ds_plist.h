
#ifndef H_PLIST
#define H_PLIST

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

typedef struct ds_plist_t ds_plist_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_plist_t *ds_plist_new (ds_plist_t *parent, const char *name);
   void ds_plist_del (ds_plist_t *plist, void *param);

   bool ds_plist_child_add (ds_plist_t *parent, ds_plist_t *child);
   void ds_plist_child_rm (ds_plist_t *parent, ds_plist_t *child);

   // For diagnostics during development. A NULL file pointer sends the output to stdout
   void ds_plist_dump (ds_plist_t *plist, FILE *outf);

   // Set name to the array of values given, which must be terminated with a NULL argument
   bool ds_plist_value_set (ds_plist_t *plist, const char *name, const char *value, ...);
   bool ds_plist_value_vset (ds_plist_t *plist, const char *name, const char *value, va_list ap);
   bool ds_plist_value_append (ds_plist_t *plist, const char *name, const char *value);

#ifdef __cplusplus
};
#endif

#endif

