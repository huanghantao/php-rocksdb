#include "php_rocksdb.h"
#include "rocksdb/db.h"

#include "error.h"

using namespace rocksdb;

typedef struct
{
    WriteBatch *batch;
    zend_object std;
} rocksdb_write_batch_t;

zend_class_entry *rocksdb_write_batch_ce;
zend_object_handlers rocksdb_write_batch_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_write_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_delete, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_put, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

static inline rocksdb_write_batch_t* php_rocksdb_write_batch_fetch_object(zend_object *obj)
{
    return (rocksdb_write_batch_t *) ((char *) obj - rocksdb_write_batch_handlers.offset);
}

WriteBatch *php_rocksdb_write_batch_get_ptr(zval *zobject)
{
    return php_rocksdb_write_batch_fetch_object(Z_OBJ_P(zobject))->batch;
}

void php_rocksdb_write_batch_set_ptr(zval *zobject, WriteBatch *batch)
{
    php_rocksdb_write_batch_fetch_object(Z_OBJ_P(zobject))->batch = batch;
}

static void php_rocksdb_write_batch_free_object(zend_object *object)
{
    zend_object_std_dtor(object);
}

static zend_object *php_rocksdb_write_batch_create_object(zend_class_entry *ce)
{
    rocksdb_write_batch_t *batch = (rocksdb_write_batch_t *) ecalloc(1, sizeof(rocksdb_write_batch_t) + zend_object_properties_size(ce));
    zend_object_std_init(&batch->std, ce);
    object_properties_init(&batch->std, ce);
    batch->std.handlers = &rocksdb_write_batch_handlers;
    return &batch->std;
}

static PHP_METHOD(rocksdb_write_batch, __construct)
{
    php_rocksdb_write_batch_set_ptr(ZEND_THIS, new WriteBatch());
}

static PHP_METHOD(rocksdb_write_batch, delete)
{
    char *key;
    size_t key_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, key_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    WriteBatch *batch = php_rocksdb_write_batch_get_ptr(ZEND_THIS);

    Status s = batch->Delete(std::string(key, key_len));
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_BATCH_DELETE_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_write_batch, put)
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_STRING(value, value_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    WriteBatch *batch = php_rocksdb_write_batch_get_ptr(ZEND_THIS);

    Status s = batch->Put(std::string(key, key_len), std::string(value, value_len));
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_BATCH_DELETE_ERROR);
    }

    RETURN_TRUE;
}

static const zend_function_entry rocksdb_write_batch_methods[] =
{
    PHP_ME(rocksdb_write_batch, __construct,  arginfo_rocksdb_write_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_write_batch, delete,  arginfo_rocksdb_delete, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_write_batch, put,  arginfo_rocksdb_put, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_rocksdb_write_batch_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_write_batch, "RocksDB\\WriteBatch", NULL, NULL, rocksdb_write_batch_methods);
    ROCKSDB_SET_CLASS_SERIALIZABLE(rocksdb_write_batch, zend_class_serialize_deny, zend_class_unserialize_deny);
    ROCKSDB_SET_CLASS_CLONEABLE(rocksdb_write_batch, rocksdb_zend_class_clone_deny);
    ROCKSDB_SET_CLASS_UNSET_PROPERTY_HANDLER(rocksdb_write_batch, rocksdb_zend_class_unset_property_deny);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_write_batch, php_rocksdb_write_batch_create_object, php_rocksdb_write_batch_free_object, rocksdb_write_batch_t, std);
}