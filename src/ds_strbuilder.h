
#ifndef H_DS_STRBUILDER
#define H_DS_STRBUILDER


typedef struct ds_strbuilder_t ds_strbuilder_t;

#ifdef __cplusplus
extern "C" {
#endif

// Create and delete a string builder instance. The parameter list must be
// terminated with a NULL pointer. The first parameter `s` can be NULL to
// construct an empty stringbuilder instance.
ds_strbuilder_t *ds_strbuilder_new (const char *s, ...);
ds_strbuilder_t *ds_strbuilder_newv (const char *s, va_list ap);
void ds_strbuilder_del (ds_strbuilder_t *sb);

// Append a new string to the string builder instance. The parameter list must
// be terminated with a NULL pointer.
bool ds_strbuilder_append (ds_strbuilder_t *sb, const char *s, ...);
bool ds_strbuilder_appendv (ds_strbuilder_t *sb, const char *s, va_list ap);

// Build the final string. Caller is responsible for freeing the returned
// value.
char *ds_strbuilder_build (ds_strbuilder_t *sb);


#ifdef __cplusplus
};
#endif


#endif


