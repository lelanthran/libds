
#ifndef H_DS_STR
#define H_DS_STR

#ifdef __cplusplus
extern "C" {
#endif

   char *ds_strdup (const char *src);
   char *ds_strcat (const char *src, ...);
   char *ds_strappend (const char *src, ...);


#ifdef __cplusplus
};
#endif

#endif

