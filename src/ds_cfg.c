#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include "ds_cfg.h"

// Dependencies on other ds_* headers is undesirable
static char *lstrdup (const char *src)
{
   if (!src)
      return NULL;
   size_t nbytes = strlen (src) + 1;
   char *ret = malloc (nbytes);
   if (!ret) {
      return NULL;
   }
   return strcpy (ret, src);
}

struct ds_cfg_t {
   size_t nrecords;
   struct ds_cfg_record_t *records;
};

ds_cfg_t *ds_cfg_new (void)
{
   ds_cfg_t *ret = calloc (1, sizeof *ret);
   if (!ret) {
      return NULL;
   }
   if (!(ret->records = calloc (1, sizeof *ret->records))) {
      free (ret);
      return NULL;
   }
   ret->nrecords = 0;
   return ret;
}

ds_cfg_t *ds_cfg_load (const char *fname, size_t max_line_len,
                       size_t *nlines,
                       int *errcode, char **errmsg)
{
   bool error = true;
   ds_cfg_t *ret = NULL;
   size_t ln = 0;

   FILE *inf = NULL;
   char *line = NULL;
   char *section = NULL;

   if (!(section = lstrdup (""))) {
      goto cleanup;
   }

   if (!(line = malloc (max_line_len))) {
      goto cleanup;
   }

   if (!(ret = ds_cfg_new ())) {
      goto cleanup;
   }

   errno = 0;

   if (!(inf = fopen (fname, "r"))) {
      fprintf (stderr, "Failed to open [%s]: %m\n", fname);
      goto cleanup;
   }

   char *tmp;
   errno = 0;
   while (!feof (inf) && !ferror(inf) && fgets (line, max_line_len, inf)) {
      ln++;
      // Do we find a newline?
      if (!(tmp = strchr (line, '\n'))) {
         // Maximum line length exceeded
         errno = EOVERFLOW;
         goto cleanup;
      }
      *tmp = 0;

      // Truncate line at the comment character to end of line
      if ((tmp = strchr (line, '#'))) {
         *tmp = 0;
      }

      // Trim the whitespace
      size_t nbytes = strlen (line);
      if (!nbytes)
         continue;

      // rtrim
      tmp = &line[nbytes - 1];
      while (tmp > line && isspace (*tmp))
         *tmp-- = 0;

      // ltrim
      tmp = line;
      while (isspace (*tmp))
         tmp++;

      // If trimming results in an empty string, ignore and continue
      if ((strlen (tmp)) == 0) {
         continue;
      }

      // If this is a section name, store it and move on to the next line
      if (*tmp == '[') {
         free (section);
         if (!(section = lstrdup (&tmp[1]))) {
            errno = ENOMEM;
            goto cleanup;
         }

         char *end = strrchr (section, ']');
         if (!end) {
            fprintf (stderr, "section malformed\n");
            errno = EINVAL;
            goto cleanup;
         }
         *end = 0;
         errno = 0;
         continue;
      }

      char *key = tmp;
      tmp = strchr (key, '=');
      if (!tmp) {
         fprintf (stderr, "key=value malformed\n");
         errno = EINVAL;
         goto cleanup;
      }
      char *value = tmp;
      *value++ = 0;

      // trim the space between '=' and the value
      while (isspace (*value))
         value++;
      // trim the space between the key and '='
      tmp--;
      while (isspace (*tmp)) {
         *tmp-- = 0;
      }

      // FINALLY! Store the damn thing
      if (!(ds_cfg_add (ret, section, key, value))) {
         errno = ENOMEM;
         goto cleanup;
      }
   }

   error = false;
cleanup:
   free (line);
   free (section);
   if (inf) {
      fclose (inf);
   }

   if (error) {
      ds_cfg_del (ret);
      ret = NULL;
   }
   if (errcode) {
      *errcode = errno;
   }
   if (errmsg) {
      *errmsg = lstrdup (strerror (errno));
   }
   if (nlines) {
      *nlines = ln;
   }


   return ret;
}


void ds_cfg_del (ds_cfg_t *cfg)
{
   if (!cfg)
      return;

   for (size_t i=0; i<cfg->nrecords; i++) {
      free (cfg->records[i].section);
      free (cfg->records[i].key);
      free (cfg->records[i].value);
   }

   free (cfg->records);
   free (cfg);
}


bool ds_cfg_add (ds_cfg_t *cfg,
                 const char *sect, const char *k, const char *v)
{
   if (!cfg)
      return false;

   size_t newsize = ds_cfg_count (cfg);

   struct ds_cfg_record_t *tmp =
      realloc (cfg->records, (newsize + 2) * (sizeof *cfg->records));

   if (!tmp) {
      return false;
   }

   cfg->records = tmp;
   cfg->nrecords = newsize + 1;
   memset (&cfg->records[newsize + 1], 0, sizeof *cfg->records);
   memset (&cfg->records[newsize], 0, sizeof *cfg->records);

   if (!(cfg->records[newsize].section = lstrdup (sect)) ||
       !(cfg->records[newsize].key = lstrdup (k)) ||
       !(cfg->records[newsize].value = lstrdup (v))) {
      free (cfg->records[newsize].section);
      free (cfg->records[newsize].key);
      free (cfg->records[newsize].value);
      return false;
   }

   return true;
}

void ds_cfg_rm (ds_cfg_t *cfg, const char *sect, const char *k)
{
   if (!cfg || !sect || !k)
      return;

   for (size_t i=0; i<cfg->nrecords; i++) {
      if ((strcmp (sect, cfg->records[i].section) == 0) &&
          (strcmp (k, cfg->records[i].key) == 0)) {
         free (cfg->records[i].section);
         free (cfg->records[i].key);
         free (cfg->records[i].value);
         memmove (&cfg->records[i], &cfg->records[i+1],
                  (cfg->nrecords - i) * (sizeof *cfg->records));
         cfg->nrecords--;
         break;
      }
   }
}


const char *ds_cfg_get (ds_cfg_t *cfg, const char *sect, const char *k)
{
   if (!cfg || !sect || !k)
      return NULL;

   for (size_t i=0; cfg->records[i].section && cfg->records[i].key; i++) {
      if ((strcmp (cfg->records[i].section, sect) == 0) &&
          (strcmp (cfg->records[i].key, k) == 0)) {
         return cfg->records[i].value;
      }
   }
   return NULL;
}


size_t ds_cfg_count (const ds_cfg_t *cfg)
{
   return cfg ? cfg->nrecords : 0;
}

struct ds_cfg_record_t *ds_cfg_all (ds_cfg_t *cfg)
{
   return cfg ? cfg->records : NULL;
}


