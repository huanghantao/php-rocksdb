/* rocksdb extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php_rocksdb.h"

#include "rocksdb/db.h"
#include "rocksdb/utilities/db_ttl.h"
#include "rocksdb/merge_operator.h"

#include "stringappend.h"
#include "error.h"

using namespace rocksdb;

typedef struct
{
    DB *db;
    DBWithTTL *db_with_ttl;
    zend_object std;
} rocksdb_container;

zend_class_entry *rocksdb_ce;
static zend_object_handlers rocksdb_handlers;

extern void php_rocksdb_iterator_set_ptr(zval *zobject, Iterator *iter);

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb__construct, 0, 0, 5)
    ZEND_ARG_INFO(0, db_name)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, ttl)
    ZEND_ARG_INFO(0, secondary_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_put, 0, 0, 3)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, writeoptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_get, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, readoptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_del, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, writeoptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_deleteRange, 0, 0, 3)
    ZEND_ARG_INFO(0, begin_key)
    ZEND_ARG_INFO(0, end_key)
    ZEND_ARG_INFO(0, writeoptions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_newIterator, 0, 0, 1)
    ZEND_ARG_INFO(0, begin_key)
    ZEND_ARG_INFO(0, readoptions)
ZEND_END_ARG_INFO()

static inline rocksdb_container *php_rocksdb_container_fetch_object(zend_object *obj)
{
    return (rocksdb_container *) ((char *) obj - rocksdb_handlers.offset);
}

static zend_object *php_rocksdb_container_create_object(zend_class_entry *ce)
{
    rocksdb_container *rocksdb_container_t = (rocksdb_container *) ecalloc(1, sizeof(rocksdb_container) + zend_object_properties_size(ce));
    zend_object_std_init(&rocksdb_container_t->std, ce);
    object_properties_init(&rocksdb_container_t->std, ce);
    rocksdb_container_t->std.handlers = &rocksdb_handlers;
    return &rocksdb_container_t->std;
}

static void php_rocksdb_container_free_object(zend_object *object)
{
    rocksdb_container *rocksdb_container_t = (rocksdb_container *) php_rocksdb_container_fetch_object(object);
    zend_object_std_dtor(&rocksdb_container_t->std);
}

static DB *php_rocksdb_db_get_ptr(zval *zobject)
{
    return php_rocksdb_container_fetch_object(Z_OBJ_P(zobject))->db;
}

static void check_rocksdb_open_options(Options &op, HashTable *vht)
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

static void check_rocksdb_write_options(WriteOptions &wop, HashTable *vht)
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

static void check_rocksdb_read_options(ReadOptions &rop, HashTable *vht)
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

PHP_RINIT_FUNCTION(rocksdb)
{
    return SUCCESS;
}

static PHP_METHOD(rocksdb, __construct)
{
    char *path;
    size_t path_len;
    zval *zoptions = nullptr;
    zend_bool mode = 0;
    zend_long ttl = 0;
    char *secondary_path;
    size_t secondary_path_len;

    ZEND_PARSE_PARAMETERS_START(1, 5)
        Z_PARAM_STRING(path, path_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zoptions)
        Z_PARAM_BOOL(mode)
        Z_PARAM_LONG(ttl)
        Z_PARAM_STRING(secondary_path, secondary_path_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    Options options;
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();

    if (zoptions)
    {
        check_rocksdb_open_options(options, Z_ARRVAL_P(zoptions));
    }

    Status s;
    if (ttl > 0 && mode == 0)
    {
        s = DBWithTTL::Open(options, path, &rocksdb_container_t->db_with_ttl, ttl);
    }
    else if (ttl > 0 && mode == 1)
    {
        s = DBWithTTL::Open(options, path, &rocksdb_container_t->db_with_ttl, ttl, true);
    }
    else if (mode == 0)
    {
        s = DB::Open(options, path, &rocksdb_container_t->db);
    }
    else if (mode == 2)
    {
        s = DB::OpenAsSecondary(options, path, secondary_path, &rocksdb_container_t->db);
    }
    else
    {
        s = DB::OpenForReadOnly(options, path, &rocksdb_container_t->db);
    }

    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_OPEN_ERROR);
    }
}

static PHP_METHOD(rocksdb, put)
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

    DB *db = php_rocksdb_db_get_ptr(ZEND_THIS);
    WriteOptions wop;

    if (zwriteoptions)
    {
        check_rocksdb_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }

    Status s = db->Put(wop, std::string(key, key_len), std::string(value, value_len));
    if (!s.ok()) {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_PUT_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb, get)
{
    char *key;
    size_t key_len;
    zval *zreadoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zreadoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    DB *db = php_rocksdb_db_get_ptr(ZEND_THIS);
    ReadOptions rop;

    if (zreadoptions)
    {
        check_rocksdb_read_options(rop, Z_ARRVAL_P(zreadoptions));
    }

    std::string value;
    Status s = db->Get(rop, std::string(key, key_len), &value);
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_GET_ERROR);
    }

    RETURN_STRINGL(value.c_str(), value.length());
}

static PHP_METHOD(rocksdb, del)
{
    char *key;
    size_t key_len;
    zval *zwriteoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zwriteoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    DB *db = php_rocksdb_db_get_ptr(ZEND_THIS);
    WriteOptions wop;

    if (zwriteoptions)
    {
        check_rocksdb_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }

    Status s = db->Delete(wop, std::string(key, key_len));
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_DELETE_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb, deleteRange)
{
    char *begin_key;
    size_t begin_key_len;
    char *end_key;
    size_t end_key_len;
    zval *zwriteoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STRING(begin_key, begin_key_len)
        Z_PARAM_STRING(end_key, end_key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zwriteoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    DB *db = php_rocksdb_db_get_ptr(ZEND_THIS);
    WriteOptions wop;

    if (zwriteoptions)
    {
        check_rocksdb_write_options(wop, Z_ARRVAL_P(zwriteoptions));
    }

    Status s = db->DeleteRange(wop, 0, std::string(begin_key, begin_key_len), std::string(end_key, end_key_len));
    if (!s.ok())
    {
        zend_throw_exception(rocksdb_exception_ce, s.ToString().c_str(), ROCKSDB_DELETE_RANGE_ERROR);
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb, newIterator)
{
    zval *begin_key;
    zval *zreadoptions = nullptr;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(begin_key)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(zreadoptions)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    DB *db = php_rocksdb_db_get_ptr(ZEND_THIS);
    ReadOptions rop;

    if (zreadoptions)
    {
        check_rocksdb_read_options(rop, Z_ARRVAL_P(zreadoptions));
    }

    zval ziter;
    object_init_ex(&ziter, rocksdb_iterator_ce);
    Iterator *iter = db->NewIterator(rop);
    php_rocksdb_iterator_set_ptr(&ziter, iter);

    zend_call_method_with_1_params(
        &ziter,
        rocksdb_iterator_ce,
        &rocksdb_iterator_ce->constructor,
        (const char *) "__construct",
        NULL,
        begin_key
    );

    RETVAL_OBJ(Z_OBJ_P(&ziter));
}

static const zend_function_entry rocksdb_methods[] =
{
    PHP_ME(rocksdb, __construct, arginfo_rocksdb__construct, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, put, arginfo_rocksdb_put, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, get, arginfo_rocksdb_get, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, del, arginfo_rocksdb_del, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, deleteRange, arginfo_rocksdb_deleteRange, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, newIterator, arginfo_rocksdb_newIterator, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(rocksdb)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb, "RocksDB", NULL, NULL, rocksdb_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb, php_rocksdb_container_create_object, php_rocksdb_container_free_object, rocksdb_container, std);

    php_rocksdb_exception_minit(module_number);
    php_rocksdb_iterator_minit(module_number);

    return SUCCESS;
}

PHP_MINFO_FUNCTION(rocksdb)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "rocksdb support", "enabled");
    php_info_print_table_end();
}

static const zend_function_entry rocksdb_functions[] = {
    PHP_FE_END
};

zend_module_entry rocksdb_module_entry = {
    STANDARD_MODULE_HEADER,
    "rocksdb",					/* Extension name */
    rocksdb_functions,			/* zend_function_entry */
    PHP_MINIT(rocksdb),							/* PHP_MINIT - Module initialization */
    NULL,							/* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(rocksdb),			/* PHP_RINIT - Request initialization */
    NULL,							/* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(rocksdb),			/* PHP_MINFO - Module info */
    PHP_ROCKSDB_VERSION,		/* Version */
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ROCKSDB
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(rocksdb)
#endif

