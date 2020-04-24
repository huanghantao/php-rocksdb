/* rocksdb extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php_rocksdb.h"

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"

#include "error.h"

using namespace rocksdb;

typedef struct
{
    Transaction *db;
    zend_object std;
} rocksdb_transaction_t;

zend_class_entry *rocksdb_transaction_ce;
static zend_object_handlers rocksdb_transaction_handlers;

extern zend_class_entry *rocksdb_snapshot_ce;

extern void check_rocksdb_db_read_options(ReadOptions &rop, HashTable *vht);

void php_rocksdb_snapshot_set_ptr(zval *zobject, const Snapshot *snapshot);

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_put, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_get, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, readOptions)
ZEND_END_ARG_INFO()

static inline rocksdb_transaction_t *php_rocksdb_transaction_fetch_object(zend_object *obj)
{
    return (rocksdb_transaction_t *) ((char *) obj - rocksdb_transaction_handlers.offset);
}

static zend_object *php_rocksdb_transaction_create_object(zend_class_entry *ce)
{
    rocksdb_transaction_t *rocksdb_transaction = (rocksdb_transaction_t *) ecalloc(1, sizeof(rocksdb_transaction_t) + zend_object_properties_size(ce));
    zend_object_std_init(&rocksdb_transaction->std, ce);
    object_properties_init(&rocksdb_transaction->std, ce);
    rocksdb_transaction->std.handlers = &rocksdb_transaction_handlers;
    return &rocksdb_transaction->std;
}

static void php_rocksdb_transaction_free_object(zend_object *object)
{
    rocksdb_transaction_t *rocksdb_transaction = (rocksdb_transaction_t *) php_rocksdb_transaction_fetch_object(object);
    zend_object_std_dtor(&rocksdb_transaction->std);
}

static Transaction *php_rocksdb_transaction_get_ptr(zval *zobject)
{
    return php_rocksdb_transaction_fetch_object(Z_OBJ_P(zobject))->db;
}

void php_rocksdb_transaction_set_ptr(zval *zobject, Transaction *db)
{
    php_rocksdb_transaction_fetch_object(Z_OBJ_P(zobject))->db = db;
}

static PHP_METHOD(rocksdb_transaction, put)
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_STRING(value, value_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Transaction *transaction = php_rocksdb_transaction_get_ptr(ZEND_THIS);

    Status s = transaction->Put(std::string(key, key_len), std::string(value, value_len));
    if (!s.ok()) {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_PUT_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_transaction, get)
{
    char *key;
    size_t key_len;
    zval *zreadoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zreadoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Transaction *transaction = php_rocksdb_transaction_get_ptr(ZEND_THIS);
    ReadOptions rop;

    if (zreadoptions)
    {
        check_rocksdb_db_read_options(rop, Z_ARRVAL_P(zreadoptions));
    }

    std::string value;
    Status s = transaction->Get(rop, std::string(key, key_len), &value);
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_GET_ERROR);
    }

    RETURN_STRINGL(value.c_str(), value.length());
}

static PHP_METHOD(rocksdb_transaction, commit)
{
    Transaction *transaction = php_rocksdb_transaction_get_ptr(ZEND_THIS);

    Status s = transaction->Commit();
    if (!s.ok()) {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_COMMIT_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_transaction, getSnapshot)
{
    Transaction *transaction = php_rocksdb_transaction_get_ptr(ZEND_THIS);

    const Snapshot *snapshot = transaction->GetSnapshot();

    zval zsnapshot;
    object_init_ex(&zsnapshot, rocksdb_snapshot_ce);
    php_rocksdb_snapshot_set_ptr(&zsnapshot, snapshot);

    RETVAL_OBJ(Z_OBJ_P(&zsnapshot));
}

static const zend_function_entry rocksdb_transaction_methods[] =
{
    PHP_ME(rocksdb_transaction, put, arginfo_rocksdb_transaction_put, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction, get, arginfo_rocksdb_transaction_get, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction, commit, arginfo_rocksdb_transaction_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction, getSnapshot, arginfo_rocksdb_transaction_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_rocksdb_transaction_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_transaction, "RocksDB\\Transaction", NULL, NULL, rocksdb_transaction_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_transaction, php_rocksdb_transaction_create_object, php_rocksdb_transaction_free_object, rocksdb_transaction_t, std);
}