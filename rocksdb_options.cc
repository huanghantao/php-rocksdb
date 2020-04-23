#include "php_rocksdb.h"

#include "rocksdb/db.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"
#include "rocksdb/merge_operator.h"

#include "stringappend.h"

using namespace rocksdb;

void check_rocksdb_db_options(Options &op, HashTable *vht)
{
    zval *ztmp;

    if (php_rocksdb_array_get_value(vht, "create_if_missing", ztmp))
    {
        op.create_if_missing = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "error_if_exists", ztmp))
    {
        op.error_if_exists = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "paranoid_checks", ztmp))
    {
        op.paranoid_checks = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "max_open_files", ztmp))
    {
        op.max_open_files = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "merge_operator", ztmp))
    {
        char *delim_char = ZSTR_VAL(zval_get_string(ztmp));
        op.merge_operator.reset(new StringAppendOperator(delim_char[0]));
    }
}

void check_rocksdb_db_write_options(WriteOptions &wop, HashTable *vht)
{
    zval *ztmp;

    if (php_rocksdb_array_get_value(vht, "disableWAL", ztmp))
    {
        wop.disableWAL = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "ignore_missing_column_families", ztmp))
    {
        wop.ignore_missing_column_families = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "low_pri", ztmp))
    {
        wop.low_pri = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "no_slowdown", ztmp))
    {
        wop.no_slowdown = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "sync", ztmp))
    {
        wop.sync = zval_is_true(ztmp);
    }
}

void check_rocksdb_transaction_db_options(TransactionDBOptions &txn_db_options, HashTable *vht)
{
    zval *ztmp;

    if (php_rocksdb_array_get_value(vht, "default_lock_timeout", ztmp))
    {
        txn_db_options.default_lock_timeout = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "max_num_deadlocks", ztmp))
    {
        txn_db_options.max_num_deadlocks = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "max_num_locks", ztmp))
    {
        txn_db_options.max_num_locks = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "num_stripes", ztmp))
    {
        txn_db_options.num_stripes = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "rollback_merge_operands", ztmp))
    {
        txn_db_options.rollback_merge_operands = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "transaction_lock_timeout", ztmp))
    {
        txn_db_options.transaction_lock_timeout = zval_get_long(ztmp);
    }
}

void check_rocksdb_db_read_options(ReadOptions &rop, HashTable *vht)
{
    zval *ztmp;

    if (php_rocksdb_array_get_value(vht, "background_purge_on_iterator_cleanup", ztmp))
    {
        rop.background_purge_on_iterator_cleanup = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "fill_cache", ztmp))
    {
        rop.fill_cache = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "ignore_range_deletions", ztmp))
    {
        rop.ignore_range_deletions = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "managed", ztmp))
    {
        rop.managed = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "pin_data", ztmp))
    {
        rop.pin_data = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "prefix_same_as_start", ztmp))
    {
        rop.prefix_same_as_start = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "tailing", ztmp))
    {
        rop.tailing = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "total_order_seek", ztmp))
    {
        rop.total_order_seek = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "verify_checksums", ztmp))
    {
        rop.verify_checksums = zval_is_true(ztmp);
    }
}