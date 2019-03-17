
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ds_str.h"

char *ds_strdup (const char *src)
{
   if (!src)
      return NULL;

   size_t len = strlen (src) + 1;

   char *ret = malloc (len);
   if (!ret)
      return NULL;

   return strcpy (ret, src);
}

char *ds_strcat (const char *src, ...)
{
   bool error = true;
   char *ret = NULL;
   const char *tmp = src;
   size_t nbytes = 0;
   va_list ap;
   va_start (ap, src);

   while (tmp) {
      nbytes += strlen (tmp);
      tmp = va_arg (ap, const char *);
   }

   va_end (ap);

   if (!(ret = malloc (nbytes + 1)))
      goto errorexit;

   *ret = 0;

   va_start (ap, src);

   tmp = src;
   while (tmp) {
      strcat (ret, tmp);
      tmp = va_arg (ap, const char *);
   }

   error = false;

errorexit:

   if (error) {
      free (ret);
      ret = NULL;
   }

   return ret;
}

char *ds_strappend (char **dst, const char *s1, ...)
{
   bool error = true;
   char *ret = NULL;
   va_list ap;

   va_start (ap, s1);

   if (!(*dst))
      (*dst) = ds_strdup ("");

   if (!(*dst))
      return NULL;

   if (!(ret = ds_strcat ((*dst), s1, NULL)))
      goto errorexit;

   while ((s1 = va_arg (ap, char *))!=NULL) {
      char *tmp = ds_strcat (ret, s1, NULL);
      if (!tmp)
         goto errorexit;

      free (ret);
      ret = tmp;
   }

   va_end (ap);

   free (*dst);
   (*dst) = ret;

   error = false;

errorexit:

   if (error) {
      free (ret);
      ret = NULL;
   }

   return ret;
}


