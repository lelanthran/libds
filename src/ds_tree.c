#include <stdlib.h>

#include "ds_tree.h"


struct ds_tree_t {
   const char *name;
   void **children;
};


ds_tree_t *ds_tree_new (ds_tree_t *parent)
{
#warning Unimplemented
   (void)parent;
   return NULL;
}

void ds_tree_del (ds_tree_t *node)
{
#warning Unimplemented
   (void)node;
}

