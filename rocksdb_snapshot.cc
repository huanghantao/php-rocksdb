/* rocksdb extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php_rocksdb.h"

#include "rocksdb/db.h"

#include "error.h"

using namespace rocksdb;

typedef struct
{
    Snapshot *snapshot;
    zend_object std;
} rocksdb_snapshot_t;

zend_class_entry *rocksdb_snapshot_ce;
static zend_object_handlers rocksdb_snapshot_handlers;

static inline rocksdb_snapshot_t *php_rocksdb_snapshot_fetch_object(zend_object *obj)
{
    return (rocksdb_snapshot_t *) ((char *) obj - rocksdb_snapshot_handlers.offset);
}

static zend_object *php_rocksdb_snapshot_create_object(zend_class_entry *ce)
{
    rocksdb_snapshot_t *rocksdb_snapshot = (rocksdb_snapshot_t *) ecalloc(1, sizeof(rocksdb_snapshot_t) + zend_object_properties_size(ce));
    zend_object_std_init(&rocksdb_snapshot->std, ce);
    object_properties_init(&rocksdb_snapshot->std, ce);
    rocksdb_snapshot->std.handlers = &rocksdb_snapshot_handlers;
    return &rocksdb_snapshot->std;
}

static void php_rocksdb_snapshot_free_object(zend_object *object)
{
    rocksdb_snapshot_t *rocksdb_snapshot = (rocksdb_snapshot_t *) php_rocksdb_snapshot_fetch_object(object);
    zend_object_std_dtor(&rocksdb_snapshot->std);
}

Snapshot *php_rocksdb_snapshot_get_ptr(zval *zobject)
{
    return php_rocksdb_snapshot_fetch_object(Z_OBJ_P(zobject))->snapshot;
}

void php_rocksdb_snapshot_set_ptr(zval *zobject, const Snapshot *snapshot)
{
    php_rocksdb_snapshot_fetch_object(Z_OBJ_P(zobject))->snapshot = (Snapshot *) snapshot;
}

static const zend_function_entry rocksdb_snapshot_methods[] =
{
    PHP_FE_END
};

void php_rocksdb_snapshot_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb_snapshot, "RocksDB\\Snapshot", NULL, NULL, rocksdb_snapshot_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb_snapshot, php_rocksdb_snapshot_create_object, php_rocksdb_snapshot_free_object, rocksdb_snapshot_t, std);
}