
#ifndef H_DS_TREE
#define H_DS_TREE

typedef struct ds_tree_t ds_tree_t;

#ifdef __cplusplus
extern "C" {
#endif

   ds_tree_t *ds_tree_new (ds_tree_t *parent);
   void ds_tree_del (ds_tree_t *node);

#ifdef __cplusplus
}
#endif

#endif
