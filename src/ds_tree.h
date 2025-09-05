
#ifndef H_DS_TREE
#define H_DS_TREE

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct ds_tree_t ds_tree_t;

typedef void (ds_tree_visitor_t) (ds_tree_t *tree);

#ifdef __cplusplus
extern "C" {
#endif

   // Creates a new node in a tree, with optional name.
   ds_tree_t *ds_tree_new (ds_tree_t *parent, const char *name);
   // Delete an existing node in the tree, returning the payload. Caller must free
   // the payload if necessary. Returns the payload from the deleted treenode.
   void *ds_tree_del (ds_tree_t *tree);

   // Append a child to the parent. Returns false on failure and true on success.
   bool ds_tree_append (ds_tree_t *parent, ds_tree_t *child);

   // Remove a child from a parent. Returns false on failure and true on success.
   bool ds_tree_remove (ds_tree_t *parent, ds_tree_t *child);

   // Set the name for the payload. Note that only true/false is returned, not the
   // previous value for this field as is done with the other field set functions.
   // Also note that the a copy is made of the name so the caller can delete this
   // parameter once this function returns.
   bool ds_tree_name_set (ds_tree_t *tree, const char *name);

   // Get the value of the name field of this treenode. Caller must not free this
   // value.
   const char *ds_tree_name_get (ds_tree_t *tree);

   // Set the payload stored in the treenode. Returns the previous payload. This
   // function cannot fail.
   void *ds_tree_payload_set (ds_tree_t *tree, void *payload);

   // Get the payload stored in the treenode.
   void *ds_tree_payload_get (const ds_tree_t *tree);

   // Set the type field of the treenode. The library does not use this payload; it
   // is only for the convenience of the caller. The previous value of the type
   // field is returned
   uint16_t ds_tree_type_set (ds_tree_t *tree, uint16_t type);

   // Get the value of the type field stored in the payload
   uint16_t ds_tree_type_get (const ds_tree_t *tree);

   // Returns the number of children.
   size_t ds_tree_nchildren (const ds_tree_t *tree);

   // Returns the nth child of the treenode.
   ds_tree_t *ds_tree_nth_child (const ds_tree_t *tree, size_t n);

   // Visits every node in the tree, no return values.
   void ds_tree_visit (const ds_tree_t *tree, ds_tree_visitor_t *fptr);

   // For testing only, dump the tree.
   void ds_tree_dump (const ds_tree_t *tree, FILE *outfile, ds_tree_visitor_t *fptr);


#ifdef __cplusplus
};
#endif


#endif


