#include <stdbool.h>

#include "ds_tree.h"

#include "ds_array.h"
#include "ds_str.h"

struct ds_tree_t {
   ds_tree_t *parent;
   uint16_t  type;
   char *name;
   ds_array_t *children; // ds_tree_t *
   void *payload;       // Managed by caller
};


ds_tree_t *ds_tree_new (ds_tree_t *parent, const char *name)
{
   bool error = true;

   ds_tree_t *ret = calloc (1, sizeof *ret);
   if (!ret)
      goto cleanup;

   if (!(ret->name = (name ? ds_str_dup (name) : ds_str_dup (""))))
      goto cleanup;

   if (!(ds_tree_append (parent, ret)))
      goto cleanup;

   if (!(ret->children = ds_array_new ()))
      goto cleanup;

   error = false;

cleanup:
   if (error) {
      ds_tree_del (ret);
      ret = NULL;
   }

   return ret;
}

void *ds_tree_del (ds_tree_t *tree)
{
   if (!tree)
      return NULL;

   ds_tree_remove (tree->parent, tree);
   free (tree->name);
   ds_tree_t **children = NULL;
   size_t nchildren = 0;
   ds_array_all (tree->children, (void ***)&children, &nchildren);
   for (size_t i=0; i<nchildren; i++) {
      ds_tree_del (children[i]);
   }
   free (children);
   ds_array_del (tree->children);

   void *payload = tree->payload;
   free (tree);
   return payload;
}

bool ds_tree_append (ds_tree_t *parent, ds_tree_t *child)
{
   if (!parent || !child)
      return true;
   return ds_array_ins_tail (parent->children, child) != NULL;
}

bool ds_tree_remove (ds_tree_t *parent, ds_tree_t *child)
{
   if (!parent || !child)
      return true;

   size_t nchildren = ds_array_length (parent->children);
   size_t found = (size_t)-1;
   for (size_t i=0; i<nchildren; i++) {
      if ((ds_array_get (parent->children, i)) == child) {
         found = i;
         break;
      }
   }

   if (found == (size_t)-1)
      return false;

   if (!(ds_array_rm (parent->children, found)))
      return false;

   child->parent = NULL;
   return true;
}

void *ds_tree_payload_set (ds_tree_t *tree, void *payload)
{
   if (!tree)
      return NULL;

   void *old = tree->payload;
   tree->payload = payload;
   return old;
}

void *ds_tree_payload_get (const ds_tree_t *tree)
{
   if (!tree)
      return NULL;
   return tree->payload;
}

uint16_t ds_tree_type_set (ds_tree_t *tree, uint16_t type)
{
   if (!tree)
      return 0;
   uint16_t old = tree->type;
   tree->type = type;
   return old;
}

uint16_t ds_tree_type_get (const ds_tree_t *tree)
{
   if (!tree)
      return 0;
   return tree->type;
}

size_t ds_tree_nchildren (const ds_tree_t *tree)
{
   if (!tree)
      return 0;
   return ds_array_length (tree->children);
}

ds_tree_t *ds_tree_nth_child (const ds_tree_t *tree, size_t n)
{
   if (!tree)
      return NULL;
   return ds_array_get (tree->children, n);
}

void ds_tree_visit (const ds_tree_t *tree, ds_tree_visitor_t *fptr)
{
   if (!tree || !fptr)
      return;

   size_t nchildren = ds_array_length (tree->children);
   for (size_t i=0; i<nchildren; i++) {
      ds_tree_t *child = ds_array_get (tree->children, i);
      fptr (child);
      ds_tree_visit (child, fptr);
   }
}

bool ds_tree_name_set (ds_tree_t *tree, const char *name)
{
   if (!tree || !name)
      return true;

   char *tmp = ds_str_dup (name);
   if (!tmp)
      return false;

   free (tree->name);
   tree->name = tmp;
   return true;
}

const char *ds_tree_name_get (ds_tree_t *tree)
{
   if (!tree)
      return "";

   return tree->name;
}

void ds_tree_dump (const ds_tree_t *tree, FILE *outfile, ds_tree_visitor_t *fptr)
{
   static int64_t depth = 0;

   if (!outfile)
      outfile = stdout;
   if (!tree)
      return;

   depth++;

   size_t nchildren = ds_array_length (tree->children);
   for (size_t i=0; i<nchildren; i++) {
      for (int64_t j=0; j<depth; j++) {
         putc (' ', outfile);
      }
      ds_tree_t *child = ds_array_get (tree->children, i);
      fptr (child);
      ds_tree_dump (child, outfile, fptr);
   }
   depth--;
}

