%module libds
%include "src/ds_array.h"
%include "src/ds_hmap.h"
%include "src/ds_ll.h"
%include "src/ds_table.h"
%{
#include "src/ds_array.h"
#include "src/ds_hmap.h"
#include "src/ds_ll.h"
#include "src/ds_table.h"

   char *ds_str_dup (const char *src);

   char *ds_str_cat (const char *src, ...);

   char *ds_str_append (char **dst, const char *s1, ...);

   size_t ds_str_printf (char **dst, const char *fmt, ...);

   char *ds_str_ltrim (char *src);
   char *ds_str_rtrim (char *src);
   char *ds_str_trim (char *src);

   char *ds_str_chsubst (const char *src, int oldc, int newc, ...);


   char *ds_str_strsubst (const char *src,
                          const char *olds, const char *news, ...);

   char *ds_str_substring (const char *src, size_t from_position, size_t nchars);
%}


