/* rocksdb extension for PHP */

#ifndef PHP_ROCKSDB_H
# define PHP_ROCKSDB_H

#include "php.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

# define PHP_ROCKSDB_VERSION "0.5.1"

extern zend_module_entry rocksdb_module_entry;
# define phpext_rocksdb_ptr &rocksdb_module_entry

void php_rocksdb_exception_minit(int module_number);
void php_rocksdb_iterator_minit(int module_number);
void php_rocksdb_write_batch_minit(int module_number);
void php_rocksdb_transaction_db_minit(int module_number);
void php_rocksdb_transaction_minit(int module_number);
void php_rocksdb_snapshot_minit(int module_number);

#ifndef ZEND_THIS
#define ZEND_THIS (&EX(This))
#endif

extern zend_class_entry *rocksdb_exception_ce;
extern zend_object_handlers rocksdb_exception_handlers;

extern zend_class_entry *rocksdb_iterator_ce;
extern zend_object_handlers rocksdb_iterator_handlers;

extern zend_class_entry *rocksdb_transaction_ce;

#define php_rocksdb_array_get_value(ht, str, v) ((v = zend_hash_str_find(ht, str, sizeof(str)-1)) && !ZVAL_IS_NULL(v))

#define ROCKSDB_SET_CLASS_CREATE(module, _create_object) \
    module##_ce->create_object = _create_object

#define ROCKSDB_SET_CLASS_FREE(module, _free_obj) \
    module##_handlers.free_obj = _free_obj

#define ROCKSDB_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj) \
    ROCKSDB_SET_CLASS_CREATE(module, _create_object); \
    ROCKSDB_SET_CLASS_FREE(module, _free_obj)

#define ROCKSDB_SET_CLASS_CUSTOM_OBJECT(module, _create_object, _free_obj, _struct, _std) \
    ROCKSDB_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj); \
    module##_handlers.offset = XtOffsetOf(_struct, _std)

#define ROCKSDB_Z_OBJCE_NAME_VAL_P(zobject) ZSTR_VAL(Z_OBJCE_P(zobject)->name)

#if PHP_VERSION_ID < 70300
/* Allocates object type and zeros it, but not the properties.
* Properties MUST be initialized using object_properties_init(). */
static inline void *zend_object_alloc(size_t obj_size, zend_class_entry *ce)
{
    void *obj = emalloc(obj_size + zend_object_properties_size(ce));
    /* Subtraction of sizeof(zval) is necessary, because zend_object_properties_size() may be
    * -sizeof(zval), if the object has no properties. */
    memset(obj, 0, obj_size - sizeof(zval));
    return obj;
}
#endif

/* PHP 7 class declaration macros */

static inline int rocksdb_zend_register_class_alias(const char *name, size_t name_len, zend_class_entry *ce)
{
    zend_string *_name;
    if (name[0] == '\\')
    {
        _name = zend_string_init(name, name_len, 1);
        zend_str_tolower_copy(ZSTR_VAL(_name), name + 1, name_len - 1);
    }
    else
    {
        _name = zend_string_init(name, name_len, 1);
        zend_str_tolower_copy(ZSTR_VAL(_name), name, name_len);
    }

    zend_string *_interned_name = zend_new_interned_string(_name);

#if PHP_VERSION_ID >= 70300
    return zend_register_class_alias_ex(ZSTR_VAL(_interned_name), ZSTR_LEN(_interned_name), ce, 1);
#else
    return zend_register_class_alias_ex(ZSTR_VAL(_interned_name), ZSTR_LEN(_interned_name), ce);
#endif
}

#define ROCKSDB_CLASS_ALIAS(name, module) do { \
    if (name) { \
        rocksdb_zend_register_class_alias(ZEND_STRL(name), module##_ce); \
    } \
} while (0)

#define ROCKSDB_CLASS_ALIAS_SHORT_NAME(shortName, module) do { \
        ROCKSDB_CLASS_ALIAS(shortName, module); \
} while (0)

#define ROCKSDB_SET_CLASS_SERIALIZABLE(module, _serialize, _unserialize) \
    module##_ce->serialize = _serialize; \
    module##_ce->unserialize = _unserialize

#define rocksdb_zend_class_clone_deny NULL
#define ROCKSDB_SET_CLASS_CLONEABLE(module, _clone_obj) \
    module##_handlers.clone_obj = _clone_obj

#define ROCKSDB_SET_CLASS_UNSET_PROPERTY_HANDLER(module, _unset_property) \
    module##_handlers.unset_property = _unset_property

#define ROCKSDB_INIT_CLASS_ENTRY_BASE(module, namespaceName, snake_name, shortName, methods, parent_ce) do { \
    zend_class_entry _##module##_ce; \
    INIT_CLASS_ENTRY(_##module##_ce, namespaceName, methods); \
    module##_ce = zend_register_internal_class_ex(&_##module##_ce, parent_ce); \
    ROCKSDB_CLASS_ALIAS(snake_name, module); \
    ROCKSDB_CLASS_ALIAS_SHORT_NAME(shortName, module); \
} while (0)

#define ROCKSDB_INIT_CLASS_ENTRY(module, namespaceName, snake_name, shortName, methods) \
    ROCKSDB_INIT_CLASS_ENTRY_BASE(module, namespaceName, snake_name, shortName, methods, NULL); \
    memcpy(&module##_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers))

#define ROCKSDB_INIT_CLASS_ENTRY_EX(module, namespaceName, snake_name, shortName, methods, parent_module) \
    ROCKSDB_INIT_CLASS_ENTRY_BASE(module, namespaceName, snake_name, shortName, methods, parent_module##_ce); \
    memcpy(&module##_handlers, &parent_module##_handlers, sizeof(zend_object_handlers))

#define ROCKSDB_INIT_CLASS_ENTRY_EX2(module, namespaceName, snake_name, shortName, methods, parent_module_ce, parent_module_handlers) \
    ROCKSDB_INIT_CLASS_ENTRY_BASE(module, namespaceName, snake_name, shortName, methods, parent_module_ce); \
    memcpy(&module##_handlers, parent_module_handlers, sizeof(zend_object_handlers))

#if PHP_VERSION_ID < 80000
static inline void rocksdb_zend_class_unset_property_deny(zval *zobject, zval *zmember, void **cache_slot)
{
    zend_class_entry *ce = Z_OBJCE_P(zobject);
    while (ce->parent)
    {
        ce = ce->parent;
    }
    if (EXPECTED(zend_hash_find(&ce->properties_info, Z_STR_P(zmember))))
    {
        zend_throw_error(NULL, "Property %s of class %s cannot be unset", Z_STRVAL_P(zmember), ROCKSDB_Z_OBJCE_NAME_VAL_P(zobject));
        return;
    }
    std_object_handlers.unset_property(zobject, zmember, cache_slot);
}
#else
static inline void rocksdb_zend_class_unset_property_deny(zend_object *object, zend_string *member, void **cache_slot)
{
    zend_class_entry *ce = object->ce;
    while (ce->parent)
    {
        ce = ce->parent;
    }
    if (EXPECTED(zend_hash_find(&ce->properties_info, member)))
    {
        zend_throw_error(NULL, "Property %s of class %s cannot be unset", ZSTR_VAL(member), ZSTR_VAL(object->ce->name));
        return;
    }
    std_object_handlers.unset_property(object, member, cache_slot);
}
#endif

# if defined(ZTS) && defined(COMPILE_DL_ROCKSDB)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_ROCKSDB_H */
