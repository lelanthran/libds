
#ifndef H_DS_JSON
#define H_DS_JSON

#include <stdio.h>

typedef struct ds_json_number_t ds_json_number_t;
typedef struct ds_json_t ds_json_t;

enum ds_json_object_type_t {
   ds_json_UNKNOWN,
   ds_json_OBJECT,
   ds_json_ARRAY,
   ds_json_STRING,
   ds_json_SYMBOL,
   ds_json_NUMBER,
};


extern size_t ds_json_nbuckets;

#ifdef __cplusplus
extern "C" {
#endif

   void ds_json_del (ds_json_t *json);

   ds_json_t *ds_json_parse_string (const char *name, const char *src);
   ds_json_t *ds_json_parse_stream (const char *name, FILE *infile);

   // Caller must free each element and the returned array.
   char **ds_json_messages_get (void);

   // Free all the messages
   void ds_json_messages_clear (void);

   // Make a human readable string; caller must free the returned value
   char *ds_json_stringify (ds_json_t *json);


#ifdef __cplusplus
};
#endif


#endif


