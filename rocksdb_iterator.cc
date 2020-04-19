#include "php_rocksdb.h"
#include "rocksdb/db.h"
#include <iostream>

using namespace rocksdb;

typedef struct
{
    Iterator *iterator;
    zend_object std;
} rocksdb_iterator_t;

zend_class_entry *rocksdb_iterator_ce;
zend_object_handlers rocksdb_iterator_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_iterator_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_iterator__construct, 0, 0, 1)
    ZEND_ARG_INFO(0, begin_key)
ZEND_END_ARG_INFO()

static PHP_METHOD(rocksdb_iterator, __construct);
static PHP_METHOD(rocksdb_iterator, __destruct);
static PHP_METHOD(rocksdb_iterator, rewind);
static PHP_METHOD(rocksdb_iterator, next);
static PHP_METHOD(rocksdb_iterator, current);
static PHP_METHOD(rocksdb_iterator, key);
static PHP_METHOD(rocksdb_iterator, valid);
static PHP_METHOD(rocksdb_iterator, count);

static inline rocksdb_iterator_t* php_rocksdb_iterator_fetch_object(zend_object *obj)
{
    return (rocksdb_iterator_t *) ((char *) obj - rocksdb_iterator_handlers.offset);
}

Iterator* php_rocksdb_iterator_get_ptr(zval *zobject)
{
    return php_rocksdb_iterator_fetch_object(Z_OBJ_P(zobject))->iterator;
}

void php_rocksdb_iterator_set_ptr(zval *zobject, Iterator *iter)
{
    php_rocksdb_iterator_fetch_object(Z_OBJ_P(zobject))->iterator = iter;
}

static void php_rocksdb_iterator_free_object(zend_object *object)
{
    zend_object_std_dtor(object);
}

static zend_object *php_rocksdb_iterator_create_object(zend_class_entry *ce)
{
    rocksdb_iterator_t *iter = (rocksdb_iterator_t *) ecalloc(1, sizeof(rocksdb_iterator_t) + zend_object_properties_size(ce));
    zend_object_std_init(&iter->std, ce);
    object_properties_init(&iter->std, ce);
    iter->std.handlers = &rocksdb_iterator_handlers;
    return &iter->std;
}

static PHP_METHOD(rocksdb_iterator, __construct)
{
    char *begin_key;
    size_t begin_key_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(begin_key, begin_key_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);
    iter->Seek(std::string(begin_key, begin_key_len));
    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_iterator, __destruct)
{
}

static PHP_METHOD(rocksdb_iterator, rewind)
{
    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);
    iter->Reset();
}

static PHP_METHOD(rocksdb_iterator, next)
{
    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);
    iter->Next();
}

static PHP_METHOD(rocksdb_iterator, current)
{
    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);
    std::string value = iter->value().ToString();

    RETURN_STRINGL(value.c_str(), value.length());
}

static PHP_METHOD(rocksdb_iterator, key)
{
    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);
    std::string key = iter->key().ToString();

    RETURN_STRINGL(key.c_str(), key.length());
}

static PHP_METHOD(rocksdb_iterator, valid)
{
    Iterator *iter = php_rocksdb_iterator_get_ptr(ZEND_THIS);

    RETURN_BOOL(iter->Valid());
}

static PHP_METHOD(rocksdb_iterator, count)
{
    php_error_docref(NULL, E_WARNING, "The count interface is not implemented");
}

static const zend_function_entry rocksdb_iterator_methods[] =
{
    PHP_ME(rocksdb_iterator, __construct,  arginfo_rocksdb_iterator__construct, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, __destruct,  arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, rewind,      arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, next,        arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, current,     arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, key,         arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, valid,       arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_iterator, count,       arginfo_rocksdb_iterator_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_rocksdb_iterator_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_iterator, "RocksDB\\Iterator", NULL, NULL, rocksdb_iterator_methods);
    ROCKSDB_SET_CLASS_SERIALIZABLE(rocksdb_iterator, zend_class_serialize_deny, zend_class_unserialize_deny);
    ROCKSDB_SET_CLASS_CLONEABLE(rocksdb_iterator, rocksdb_zend_class_clone_deny);
    ROCKSDB_SET_CLASS_UNSET_PROPERTY_HANDLER(rocksdb_iterator, rocksdb_zend_class_unset_property_deny);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_iterator, php_rocksdb_iterator_create_object, php_rocksdb_iterator_free_object, rocksdb_iterator_t, std);
    zend_class_implements(rocksdb_iterator_ce, 1, zend_ce_iterator);
}