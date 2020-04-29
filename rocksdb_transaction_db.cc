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
    TransactionDB *db;
    zend_object std;
} rocksdb_transaction_db_t;

zend_class_entry *rocksdb_transaction_db_ce;
static zend_object_handlers rocksdb_transaction_db_handlers;

extern void php_rocksdb_transaction_set_ptr(zval *zobject, Transaction *db);

extern void check_rocksdb_db_options(Options &op, HashTable *vht);
extern void check_rocksdb_db_write_options(WriteOptions &wop, HashTable *vht);
extern void check_rocksdb_db_read_options(ReadOptions &rop, HashTable *vht);
extern void check_rocksdb_transaction_db_options(TransactionDBOptions &txn_db_options, HashTable *vht);
extern void check_rocksdb_transaction_options(TransactionOptions &txn_options, HashTable *vht);

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_open, 0, 0, 3)
    ZEND_ARG_INFO(0, dbName)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, txnDBOptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_beginTransaction, 0, 0, 2)
    ZEND_ARG_INFO(0, writeOptions)
    ZEND_ARG_INFO(0, txnOptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_put, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, writeOptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_get, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, readOptions)
ZEND_END_ARG_INFO()

static inline rocksdb_transaction_db_t *php_rocksdb_transaction_db_fetch_object(zend_object *obj)
{
    return (rocksdb_transaction_db_t *) ((char *) obj - rocksdb_transaction_db_handlers.offset);
}

static zend_object *php_rocksdb_transaction_db_create_object(zend_class_entry *ce)
{
    rocksdb_transaction_db_t *rocksdb_transaction_db = (rocksdb_transaction_db_t *) zend_object_alloc(sizeof(rocksdb_transaction_db_t), ce);
    zend_object_std_init(&rocksdb_transaction_db->std, ce);
    object_properties_init(&rocksdb_transaction_db->std, ce);
    rocksdb_transaction_db->std.handlers = &rocksdb_transaction_db_handlers;
    return &rocksdb_transaction_db->std;
}

static void php_rocksdb_transaction_db_free_object(zend_object *object)
{
    rocksdb_transaction_db_t *rocksdb_transaction_db = (rocksdb_transaction_db_t *) php_rocksdb_transaction_db_fetch_object(object);
    zend_object_std_dtor(&rocksdb_transaction_db->std);
}

static TransactionDB *php_rocksdb_transaction_db_get_ptr(zval *zobject)
{
    return php_rocksdb_transaction_db_fetch_object(Z_OBJ_P(zobject))->db;
}

static PHP_METHOD(rocksdb_transaction_db, __construct)
{
    
}

static PHP_METHOD(rocksdb_transaction_db, open)
{
    char *path;
    size_t path_len;
    zval *zoptions = nullptr;
    zval *ztxn_db_options = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STRING(path, path_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zoptions)
        Z_PARAM_ARRAY(ztxn_db_options)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_transaction_db_t *rocksdb_transaction_db = php_rocksdb_transaction_db_fetch_object(Z_OBJ_P(ZEND_THIS));

    Options options;
    TransactionDBOptions txn_db_options;

    if (zoptions)
    {
        check_rocksdb_db_options(options, Z_ARRVAL_P(zoptions));
    }
    if (ztxn_db_options)
    {
        check_rocksdb_transaction_db_options(txn_db_options, Z_ARRVAL_P(ztxn_db_options));
    }

    Status s;
    
    s = TransactionDB::Open(options, txn_db_options, path, &rocksdb_transaction_db->db);

    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_OPEN_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_transaction_db, beginTransaction)
{
    zval *zwriteoptions = nullptr;
    zval *ztxn_options = nullptr;

    ZEND_PARSE_PARAMETERS_START(0, 2)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zwriteoptions)
        Z_PARAM_ARRAY(ztxn_options)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    TransactionDB *txn_db = php_rocksdb_transaction_db_get_ptr(ZEND_THIS);
    WriteOptions wop;
    TransactionOptions txn_options;

    if (zwriteoptions)
    {
        check_rocksdb_db_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }
    if (ztxn_options)
    {
        check_rocksdb_transaction_options(txn_options, Z_ARRVAL_P(ztxn_options));
    }

    Transaction *txn = txn_db->BeginTransaction(wop, txn_options);

    zval ztransaction;
    object_init_ex(&ztransaction, rocksdb_transaction_ce);
    php_rocksdb_transaction_set_ptr(&ztransaction, txn);

    RETVAL_OBJ(Z_OBJ_P(&ztransaction));
}

static PHP_METHOD(rocksdb_transaction_db, put)
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;
    zval *zwriteoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_STRING(value, value_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zwriteoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    TransactionDB *db = php_rocksdb_transaction_db_get_ptr(ZEND_THIS);
    WriteOptions wop;

    if (zwriteoptions)
    {
        check_rocksdb_db_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }

    Status s = db->Put(wop, std::string(key, key_len), std::string(value, value_len));
    if (!s.ok()) {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_PUT_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb_transaction_db, get)
{
    char *key;
    size_t key_len;
    zval *zreadoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zreadoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    TransactionDB *txn_db = php_rocksdb_transaction_db_get_ptr(ZEND_THIS);
    ReadOptions rop;

    if (zreadoptions)
    {
        check_rocksdb_db_read_options(rop, Z_ARRVAL_P(zreadoptions));
    }

    std::string value;
    Status s = txn_db->Get(rop, std::string(key, key_len), &value);
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_GET_ERROR);
    }

    RETURN_STRINGL(value.c_str(), value.length());
}

static PHP_METHOD(rocksdb_transaction_db, close)
{
    TransactionDB *txn_db = php_rocksdb_transaction_db_get_ptr(ZEND_THIS);
    Status s = txn_db->Close();

    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_CLOSE_ERROR);
    }
    RETURN_TRUE;
}

static const zend_function_entry rocksdb_transaction_db_methods[] =
{
    PHP_ME(rocksdb_transaction_db, __construct,  arginfo_rocksdb_transaction_db_void, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, open,  arginfo_rocksdb_transaction_db_open, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, beginTransaction,  arginfo_rocksdb_transaction_db_beginTransaction, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, put,  arginfo_rocksdb_transaction_db_put, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, get,  arginfo_rocksdb_transaction_db_get, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, close,  arginfo_rocksdb_transaction_db_void, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_rocksdb_transaction_db_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_transaction_db, "RocksDB\\TransactionDB", NULL, NULL, rocksdb_transaction_db_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_transaction_db, php_rocksdb_transaction_db_create_object, php_rocksdb_transaction_db_free_object, rocksdb_transaction_db_t, std);
}