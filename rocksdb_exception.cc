#include "php_rocksdb.h"

zend_class_entry *rocksdb_exception_ce;
zend_object_handlers rocksdb_exception_handlers;

void php_rocksdb_exception_minit(int module_number)
{
    ROCKSDB_INIT_CLASS_ENTRY_EX2(rocksdb_exception, "RocksDB\\Exception", NULL, NULL, NULL, zend_ce_exception, zend_get_std_object_handlers());
}