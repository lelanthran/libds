
#ifndef H_DS_STR
#define H_DS_STR

#include <stdarg.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

   // Make a copy of the src string, caller must free the result, NULL
   // returned on error.
   char *ds_str_dup (const char *src);

   // Concatenate all the strings given (ending the parameter list with a
   // NULL) into a single string that is returned which the caller must
   // free. NULL is returned on error
   char *ds_str_cat (const char *src, ...);
   char *ds_str_vcat (const char *src, va_list ap);

   // Append all the strings given in '...' (ending with a NULL) to
   // parameter '(*dst)'. Parameter '(*dst)' is reallocated as necessary
   // and therefore must be reallocatable (returned by free() or similar).
   // The reallocated '(*dst)' is also returned on success.
   //
   // NULL is returned on error.
   char *ds_str_append (char **dst, const char *s1, ...);
   char *ds_str_vappend (char **dst, const char *s1, va_list ap);

   // Perform a printf into a buffer allocated on demand. The parameter
   // '*dst' is allocated by this function and must be freed by the caller.
   // On success the length of '*dst' is returned, on failure zero is
   // returned.
   size_t ds_str_printf (char **dst, const char *fmt, ...);
   size_t ds_str_vprintf (char **dst, const char *fmt, va_list ap);

   // Perform a character substitution on the source string. The caller
   // must free the returned value. NULL is returned on error.
   //
   // All occurrences of 'oldc' will be replaced with 'newc'. Thereafter
   // every two arguments will be interpreted as a new {oldc,newc} pair
   // until oldc is 0.
   //
   // Note that although oldc and newc are both of type int, they are cast
   // to char before usage.
   char *ds_str_chsubst (const char *src, int oldc, int newc, ...);
   char *ds_str_vchsubst (const char *src, int oldc, int newc, va_list ap);

#ifdef __cplusplus
};
#endif

#endif

