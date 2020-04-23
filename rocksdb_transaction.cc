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

static const zend_function_entry rocksdb_transaction_methods[] =
{
    PHP_FE_END
};

void php_rocksdb_transaction_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_transaction, "RocksDB\\Transaction", NULL, NULL, rocksdb_transaction_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_transaction, php_rocksdb_transaction_create_object, php_rocksdb_transaction_free_object, rocksdb_transaction_t, std);
}