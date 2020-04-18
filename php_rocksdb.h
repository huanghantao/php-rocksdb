/* rocksdb extension for PHP */

#ifndef PHP_ROCKSDB_H
# define PHP_ROCKSDB_H

extern zend_module_entry rocksdb_module_entry;
# define phpext_rocksdb_ptr &rocksdb_module_entry

# define PHP_ROCKSDB_VERSION "0.1.0"

#ifndef ZEND_THIS
#define ZEND_THIS (&EX(This))
#endif

#define php_rocksdb_array_get_value(ht, str, v) ((v = zend_hash_str_find(ht, str, sizeof(str)-1)) && !ZVAL_IS_NULL(v))

#define ROCKSDB_SET_CLASS_CREATE(module, _create_object) \
    module##_ce_ptr->create_object = _create_object

#define ROCKSDB_SET_CLASS_FREE(module, _free_obj) \
    module##_handlers.free_obj = _free_obj

#define ROCKSDB_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj) \
    ROCKSDB_SET_CLASS_CREATE(module, _create_object); \
    ROCKSDB_SET_CLASS_FREE(module, _free_obj)

#define ROCKSDB_SET_CLASS_CUSTOM_OBJECT(module, _create_object, _free_obj, _struct, _std) \
    ROCKSDB_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj); \
    module##_handlers.offset = XtOffsetOf(_struct, _std)

# if defined(ZTS) && defined(COMPILE_DL_ROCKSDB)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_ROCKSDB_H */
