
#ifndef H_DS_KVPAIR
#define H_DS_KVPAIR


typedef struct ds_kvpair_t ds_kvpair_t;

#ifdef __cplusplus
extern "C" {
#endif

  ds_kvpair_t *ds_kvpair_new (const char *key, const char *value);
  void ds_kvpair_del (ds_kvpair_t *kv);

  const char *ds_kvpair_key_get (const ds_kvpair_t *kv);
  const char *ds_kvpair_key_set (ds_kvpair_t *kv, const char *key);

  const char *ds_kvpair_value_get (const ds_kvpair_t *kv);
  const char *ds_kvpair_value_set (ds_kvpair_t *kv, const char *value);


#ifdef __cplusplus
};
#endif


#endif


