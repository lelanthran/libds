
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

   ds_json_t *ds_json_parse_string (const char *src);
   ds_json_t *ds_json_parse_stream (FILE *infile);


#ifdef __cplusplus
};
#endif


#endif


