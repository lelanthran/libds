#include <stdlib.h>


#include "ds_symtree.h"
#include "ds_array.h"
#include "ds_str.h"


typedef void *(value_init_t) (const void *src);
typedef void (value_del_t) (void **src);
typedef char *(value_2json_t) (const void *src, size_t indent);


static void *value_init_STRING (const void *src)
{
   return src ? ds_str_cat ("\"", src, "\"", NULL) : ds_str_dup ("\"\"");
}

static void value_del_STRING (void **src)
{
   if (!src || !*src)
      return;
   free (*src);
   *src = NULL;
}

static char *value_2json_STRING (const void *src, size_t indent)
{
   (void)indent;
   return ds_str_cat (src, NULL);
}



static void *value_init_SYMBOL (const void *src)
{
   return ds_str_dup (src);;
}

static void value_del_SYMBOL (void **src)
{
   value_del_STRING (src);
}

static char *value_2json_SYMBOL (const void *src, size_t indent)
{
   return value_2json_STRING (src, indent);
}



static void *value_init_ARRAY (const void *src)
{
   (void)src;
   return ds_array_new ();
}

static void value_del_ARRAY (void **src)
{
   if (!src || !*src)
      return;

   ds_array_t *arr = *src;
   size_t nchildren = ds_array_length (arr);

   for (size_t i=0; i<nchildren; i++) {
      ds_symtree_t *child = ds_array_get (arr, i);
      ds_symtree_del (&child);
   }
   ds_array_del (arr);
   *src = NULL;
}

static char *value_2json_AO (const char *sdelim, const char *edelim,
                             const void *src, size_t indent)
{
   bool error = true;
   const ds_array_t *arr = src;
   size_t nelems = ds_array_length (arr);
   char *ret = NULL;
   char *tmp = NULL;
   const char *delim = "";
   char *indent_spaces = NULL;

   indent++;

   for (size_t i=0; i<indent; i++) {
      ds_str_append (&indent_spaces, "   ", NULL);
   }

   if (!(ret = ds_str_cat ("\n", indent_spaces, sdelim, NULL)))
      goto cleanup;

   for (size_t i=0; i<nelems; i++) {
      ds_symtree_t *child = ds_array_get (arr, i);
      free (tmp);
      if (!(tmp = ds_symtree_2json (child, indent + 2)))
         goto cleanup;
      if (!(ds_str_append (&ret, delim, "\n", indent_spaces, tmp, NULL)))
         goto cleanup;
      delim = ", ";
   }
   if (!(ds_str_append (&ret, "\n", indent_spaces, edelim, NULL)))
      goto cleanup;

   error = false;
cleanup:
   free (tmp);
   free (indent_spaces);
   if (error) {
      free (ret);
      ret = NULL;
   }

   return ret;
}

static char *value_2json_ARRAY (const void *src, size_t indent)
{
   return value_2json_AO ("[", "]", src, indent);
}

static char *value_2json_OBJECT (const void *src, size_t indent)
{
   return value_2json_AO ("{", "}", src, indent);
}








struct ds_symtree_t {
   // Optional; will be NULL for root node
   ds_symtree_t *parent;

   // Optional; can have anonymous objects or arrays
   char *name;

   // Store the type and functions to operate on the value
   enum ds_symtree_type_t type;
   void *value;

   value_init_t *finit;
   value_del_t *fdel;
   value_2json_t *f2json;
};


ds_symtree_t *ds_symtree_new (ds_symtree_t *parent,
                              enum ds_symtree_type_t type,
                              const char *node_name,
                              const char *node_value)
{
   bool error = true;
   ds_symtree_t *ret = calloc (1, sizeof *ret);
   if (!ret)
      goto cleanup;

   if (node_name && !(ret->name = ds_str_dup (node_name))) {
      goto cleanup;
   }

   if (parent) {
      if (parent->type != ds_symtree_ARRAY && parent->type != ds_symtree_OBJECT)
         goto cleanup;
      if (!(ds_array_ins_tail (parent->value, ret)))
         goto cleanup;
      ret->parent = parent;
   }

   ret->type = type;
   switch (ret->type) {
      case ds_symtree_NONE:
         goto cleanup;
      case ds_symtree_STRING:
         ret->finit = value_init_STRING;
         ret->fdel = value_del_STRING;
         ret->f2json = value_2json_STRING;
         break;
      case ds_symtree_SYMBOL:
         ret->finit = value_init_SYMBOL;
         ret->fdel = value_del_SYMBOL;
         ret->f2json = value_2json_SYMBOL;
         break;
      case ds_symtree_ARRAY:
         ret->finit = value_init_ARRAY;
         ret->fdel = value_del_ARRAY;
         ret->f2json = value_2json_ARRAY;
         break;
      case ds_symtree_OBJECT:
         ret->finit = value_init_ARRAY;
         ret->fdel = value_del_ARRAY;
         ret->f2json = value_2json_OBJECT;
         break;
   }

   ret->value = ret->finit (node_value);

   error = false;
cleanup:
   if (error) {
      ds_symtree_del (&ret);
   }

   return ret;
}

void ds_symtree_del (ds_symtree_t **node)
{
   if (!node || !*node)
      return;

   free ((*node)->name);
   (*node)->fdel (&(*node)->value);
   free (*node);
   *node = NULL;
}


bool ds_symtree_name_set (ds_symtree_t *node, const char *newnode)
{
   if (!node || !newnode)
      return false;

   char *tmp = ds_str_dup (newnode);
   if (!tmp)
      return false;

   free (node->name);
   node->name = tmp;
   return true;
}

bool ds_symtree_value_set (ds_symtree_t *node, const char *value)
{
   if (!node || node->type == ds_symtree_ARRAY || node->type == ds_symtree_OBJECT)
      return false;
   char *newval = (node->type == ds_symtree_STRING)
                     ? ds_str_cat ("\"", value, "\"", NULL)
                     : ds_str_dup (value);
   if (!newval)
      return false;

   free (node->value);
   node->value = newval;
   return true;
}


enum ds_symtree_type_t ds_symtree_type_get (const ds_symtree_t *node)
{
   return node ? node->type : ds_symtree_NONE;
}

char *ds_symtree_2json (ds_symtree_t *node, size_t indent)
{
   bool error = true;
   char *ret = NULL;
   const char *delim = node->value ? ": " : "";
   if (node->name) {
      if (!(ret = ds_str_cat ("\"", node->name, "\"", delim,  NULL)))
         return NULL;
   }
   char *tmp = node->f2json (node->value, indent);
   if (!(ds_str_append (&ret, tmp, NULL)))
      goto cleanup;

   error = false;
cleanup:
   free (tmp);
   if (error) {
      free (ret);
      ret = NULL;
   }

   return ret;
}


