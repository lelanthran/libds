
#ifndef H_DS_STR
#define H_DS_STR

#ifdef __cplusplus
extern "C" {
#endif

   // Make a copy of the src string, caller must free the result, NULL
   // returned on error.
   char *ds_strdup (const char *src);

   // Concatenate all the strings given (ending the parameter list with a
   // NULL) into a single string that is returned which the caller must
   // free. NULL is returned on error
   char *ds_strcat (const char *src, ...);

   // Append all the strings given in '...' (ending with a NULL) to
   // parameter '(*dst)'. Parameter '(*dst)' is reallocated as necessary
   // and therefore must be reallocatable (returned by free() or similar).
   // The reallocated '(*dst)' is also returned on success.
   //
   // NULL is returned on error.
   char *ds_strappend (char **dst, const char *s1, ...);


#ifdef __cplusplus
};
#endif

#endif

