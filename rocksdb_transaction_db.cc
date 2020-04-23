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

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db__construct, 0, 0, 3)
    ZEND_ARG_INFO(0, dbName)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, txnDBOptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_transaction_db_beginTransaction, 0, 0, 1)
    ZEND_ARG_INFO(0, writeOptions)
ZEND_END_ARG_INFO()

static inline rocksdb_transaction_db_t *php_rocksdb_transaction_db_fetch_object(zend_object *obj)
{
    return (rocksdb_transaction_db_t *) ((char *) obj - rocksdb_transaction_db_handlers.offset);
}

static zend_object *php_rocksdb_transaction_db_create_object(zend_class_entry *ce)
{
    rocksdb_transaction_db_t *rocksdb_transaction_db = (rocksdb_transaction_db_t *) ecalloc(1, sizeof(rocksdb_transaction_db_t) + zend_object_properties_size(ce));
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
}

static PHP_METHOD(rocksdb_transaction_db, beginTransaction)
{
    zval *zwriteoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zwriteoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    TransactionDB *txn_db = php_rocksdb_transaction_db_get_ptr(ZEND_THIS);
    WriteOptions wop;

    if (zwriteoptions)
    {
        check_rocksdb_db_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }

    Transaction *txn = txn_db->BeginTransaction(wop);

    zval ztransaction;
    object_init_ex(&ztransaction, rocksdb_transaction_ce);
    php_rocksdb_transaction_set_ptr(&ztransaction, txn);

    RETVAL_OBJ(Z_OBJ_P(&ztransaction));
}

static const zend_function_entry rocksdb_transaction_db_methods[] =
{
    PHP_ME(rocksdb_transaction_db, __construct,  arginfo_rocksdb_transaction_db__construct, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb_transaction_db, beginTransaction,  arginfo_rocksdb_transaction_db_beginTransaction, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void php_rocksdb_transaction_db_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_transaction_db, "RocksDB\\TransactionDB", NULL, NULL, rocksdb_transaction_db_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_transaction_db, php_rocksdb_transaction_db_create_object, php_rocksdb_transaction_db_free_object, rocksdb_transaction_db_t, std);
}