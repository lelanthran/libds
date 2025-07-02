
#ifndef H_SYMTREE
#define H_SYMTREE

#include <stdbool.h>

/* A tree that stores name/value pairs, where the value of each  node is one of the
 * following:
 *    1. A string (indicates that value must be quoted when serialised to a string)
 *    2. A symbol (indicates that value must not be quoted when serialised to a
 *       string)
 *    4. An array of nodes
 *
 * No error checking is performed. As one example, if the caller specifies an object
 * without a name, it will be emitted as an object literal, without a name. The
 * caller must ensure that the tree is constructed correctly with the correct types.
 */

typedef struct ds_symtree_t  ds_symtree_t;
enum ds_symtree_type_t {
   ds_symtree_NONE   = 0,
   ds_symtree_STRING = 1,
   ds_symtree_SYMBOL = 2,
   ds_symtree_ARRAY  = 3,
   ds_symtree_OBJECT = 4,
};

#ifdef __cplusplus
extern "C" {
#endif

   ds_symtree_t *ds_symtree_new (ds_symtree_t *parent,
                                 enum ds_symtree_type_t type,
                                 const char *node_name);
   void ds_symtree_del (ds_symtree_t **node);

   bool ds_symtree_name_set (ds_symtree_t *node, const char *newnode);
   bool ds_symtree_value_set (ds_symtree_t *node, const char *value);

   void ds_symtree_type_set (const ds_symtree_t *node, enum ds_symtree_type_t type);
   enum ds_symtree_type_t ds_symtree_type_get (const ds_symtree_t *node);

   char *ds_symtree_2json (ds_symtree_t *node, size_t indent);

#ifdef __cplusplus
};
#endif


#endif


