
#include <stdio.h>
#include <stdlib.h>

#include "ds_cfg.h"

#define TESTFILE     "./testfiles/ds_cfg_test.cfg"
#define MAX_LINE_LEN (1024 * 1024 * 8)

int main (void)
{
   int ret = EXIT_FAILURE;

   char *errmsg = NULL;
   int errcode = 0;
   size_t nlines = 0;
   ds_cfg_t *cfg = ds_cfg_load (TESTFILE, MAX_LINE_LEN, &nlines, &errcode, &errmsg);
   struct ds_cfg_record_t *all = NULL;

   if (!cfg) {
      fprintf (stderr, "%s:%zu: Configuration error: %i (%s)\n",
            TESTFILE, nlines, errcode, errmsg);
      goto cleanup;
   }

   if (!(all = ds_cfg_all (cfg))) {
      fprintf (stderr, "Failed to retrieve all the keys\n");
      goto cleanup;
   }

   const char *value = ds_cfg_get (cfg, "section1", "key3.3");
   if (!value) {
      fprintf (stderr, "Failed to find key3.3 in config\n");
      goto cleanup;
   }
   fprintf (stdout, "Found key3.3 [%s]\n", value);

   ds_cfg_rm (cfg, "section1", "key3.4");

   fprintf (stdout, "Found %zu kvpairs\n", ds_cfg_count (cfg));
   for (size_t i=0; all[i].section && all[i].key && all[i].value; i++) {
      fprintf (stdout, "%s:%s:%s\n", all[i].section, all[i].key, all[i].value);
   }



   ret = EXIT_SUCCESS;
cleanup:
   free (errmsg);
   ds_cfg_del (cfg);

   return ret;
}

