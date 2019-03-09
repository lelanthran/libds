
#ifndef H_DATASTRUCTURE_LL
#define H_DATASTRUCTURE_LL

#ifdef __cplusplus
extern "C" {
#endif

   void **ds_array_new (void);
   void ds_array_del (void **ll);
   void **ds_array_copy (void **src, size_t from_index, size_t to_index);

   size_t ds_array_length (void **ll);
   const void *ds_array_index (void **ll, size_t i);
   void ds_array_iterate (void **ll, void (*fptr) (const void *));

   const void *ds_array_ins_tail (void ***ll, const void *el);
   const void *ds_array_ins_head (void ***ll, const void *el);

   const void *ds_array_remove_tail (void ***ll);
   const void *ds_array_remove_head (void ***ll);

   const void *ds_array_remove (void ***ll, size_t index);

#ifdef __cplusplus
};
#endif

#endif


