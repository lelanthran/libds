
#ifndef H_DS_JSON
#define H_DS_JSON

#include <stdio.h>
#include <stdarg.h>

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
   char *ds_json_stringify (const ds_json_t *json);

   // Get the type of the object
   enum ds_json_object_type_t ds_json_type (const ds_json_t *json);

   // Gets the fieldnames of the object, IFF it is of type ds_json_OBJECT. On error
   // or if the specified object is not a ds_json_OBJECT type, returns NULL.
   // Caller must free the returned array as well as all the elements of the array
   char **ds_json_fieldnames (const ds_json_t *json);

   // Gets the value from an array at the specified index, or NULL if the specified
   // json object is not an array or if the index is out of range
   ds_json_t *ds_json_get_index (const ds_json_t *json, size_t index);

   // Retrieve a value from an object in two different ways:
   // 1. A null-terminated array of pointers to each path component
   // 2. A null-terminated parameter list to each path component
   const ds_json_t *ds_json_geta (const ds_json_t *obj, char **path);
   const ds_json_t *ds_json_getv (const ds_json_t *obj, va_list ap);
   const ds_json_t *ds_json_get (const ds_json_t *obj, ...);

#ifdef __cplusplus
};
#endif


#endif


