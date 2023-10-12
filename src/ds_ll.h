/* ********************************************************
 * Copyright ©2019 Lelanthran Manickum, All rights reserved
 * This project  is licensed under the MIT License. See the
 * file LICENSE for more information.
 */


#ifndef H_DS_LL
#define H_DS_LL

#include <stdlib.h>

typedef struct ds_ll_t ds_ll_t;

#ifdef __cplusplus
extern "C" {
#endif

   // Delete a single node, unlinking it from its neighbours if
   // necessary Note that the element remains untouched (it is not
   // deleted) and remains the caller's responsibility.
   void ds_ll_remove (ds_ll_t *node);

   // Same as ds_ll_remove(), with the difference being that *ALL* nodes in
   // the linked list is deleted. Any node linked directly or indirectly
   // to this node is deleted. The actual elements are not deleted and
   // remain the responsibility of the caller.
   void ds_ll_del_all (ds_ll_t *node);

   // Create a new linked list or insert a node into an existing one using
   // the specified 'value' as the payload of the node.
   //
   // If the prev_node/next_node value is NULL then a new unlinked
   // individual node is created. If the prev_node/next_node is not NULL
   // then the new node that is created is linked after/before the
   // specified prev_node/next_node respectively.
   ds_ll_t *ds_ll_ins_after (ds_ll_t *prev_node, void *el);
   ds_ll_t *ds_ll_ins_before (ds_ll_t *next_node, void *el);

   // Insert a new node into the list at the tail or at the head
   // respectively, using the provided 'el' as the value of the new node.
   // If the provided node is not already a tail or head node the list is
   // looped over until a tail or head node is reached (whichever is
   // needed).
   //
   // The inserted node is returned on success, and NULL is returned on
   // failure.
   ds_ll_t *ds_ll_ins_tail (ds_ll_t *node, void *el);
   ds_ll_t *ds_ll_ins_head (ds_ll_t *node, void *el);

   // Return the value stored at the specified node.
   void *ds_ll_value (ds_ll_t *node);

   // Return the first, the last, the next or the previous node in the
   // linked list that the specified 'node' is part of. NULL is returned
   // on error.
   ds_ll_t *ds_ll_first (ds_ll_t *node);
   ds_ll_t *ds_ll_last  (ds_ll_t *node);
   ds_ll_t *ds_ll_next  (ds_ll_t *node);
   ds_ll_t *ds_ll_prev  (ds_ll_t *node);

   // Return the next i'th node (as specified by 'index') forward from the
   // current node. The current node is node zero, the next node is 1,
   // etc. If the list ends before a node is found then NULL is returned.
   ds_ll_t *ds_ll_forward (ds_ll_t *node, size_t index);

   // Return the previous i'th node (as specified by 'index') backward from
   // the current node. The current node is node zero, the previous node
   // is 1, etc. If the list ends before a node is found then NULL is
   // returned.
   ds_ll_t *ds_ll_back (ds_ll_t *node, size_t index);

#ifdef __cplusplus
};
#endif

#endif


