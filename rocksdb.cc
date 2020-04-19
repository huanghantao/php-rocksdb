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
    WriteOptions *write_options;
    ReadOptions *read_options;
    zend_object std;
} rocksdb_container;

zend_class_entry *rocksdb_ce;
static zend_object_handlers rocksdb_handlers;

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb__construct, 0, 0, 7)
    ZEND_ARG_INFO(0, db_name)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, readoptions)
    ZEND_ARG_INFO(0, writeoptions)
    ZEND_ARG_INFO(0, mode)
    ZEND_ARG_INFO(0, ttl)
    ZEND_ARG_INFO(0, secondary_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_put, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_get, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_del, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_rocksdb_deleteRange, 0, 0, 1)
    ZEND_ARG_INFO(0, begin_key)
    ZEND_ARG_INFO(0, end_key)
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

PHP_RINIT_FUNCTION(rocksdb)
{
    return SUCCESS;
}

static PHP_METHOD(rocksdb, __construct)
{
    char *path;
    size_t path_len;
    zval *zoptions;
    zval *zreadoptions;
    zval *zwriteoptions;
    zend_bool mode = 0;
    zend_long ttl = 0;
    char *secondary_path;
    size_t secondary_path_len;

    zval *ztmp;
    HashTable *vht;

    ZEND_PARSE_PARAMETERS_START(4, 7)
        Z_PARAM_STRING(path, path_len)
        Z_PARAM_ARRAY(zoptions)
        Z_PARAM_ARRAY(zreadoptions)
        Z_PARAM_ARRAY(zwriteoptions)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(mode)
        Z_PARAM_LONG(ttl)
        Z_PARAM_STRING(secondary_path, secondary_path_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Options options;
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();

    vht = Z_ARRVAL_P(zoptions);

    if (php_rocksdb_array_get_value(vht, "create_if_missing", ztmp))
    {
        options.create_if_missing = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "error_if_exists", ztmp))
    {
        options.error_if_exists = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "paranoid_checks", ztmp))
    {
        options.paranoid_checks = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "max_open_files", ztmp))
    {
        options.max_open_files = zval_get_long(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "merge_operator", ztmp))
    {
        char *delim_char = ZSTR_VAL(zval_get_string(ztmp));
        options.merge_operator.reset(new StringAppendOperator(delim_char[0]));
    }

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    Status s;
    if (ttl > 0 && mode == 0) {
        s = DBWithTTL::Open(options, path, &rocksdb_container_t->db_with_ttl, ttl);
    } else if (ttl > 0 && mode == 1) {
        s = DBWithTTL::Open(options, path, &rocksdb_container_t->db_with_ttl, ttl, true);
    } else if (mode == 0) {
        s = DB::Open(options, path, &rocksdb_container_t->db);
    } else if (mode == 2) {
        s = DB::OpenAsSecondary(options, path, secondary_path, &rocksdb_container_t->db);
    } else {
        s = DB::OpenForReadOnly(options, path, &rocksdb_container_t->db);
    }

    if (!s.ok()) {
        std::string name = "RocksDB open failed msg: " + s.ToString();
        zend_throw_exception(rocksdb_ce, name.c_str(), ROCKSDB_OPEN_ERROR);
        RETURN_FALSE;
    }

    vht = Z_ARRVAL_P(zwriteoptions);
    rocksdb_container_t->write_options = new WriteOptions();
    if (php_rocksdb_array_get_value(vht, "sync", ztmp))
    {
        rocksdb_container_t->write_options->sync = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "disableWAL", ztmp))
    {
        rocksdb_container_t->write_options->disableWAL = zval_is_true(ztmp);
    }

    vht = Z_ARRVAL_P(zreadoptions);
    rocksdb_container_t->read_options = new ReadOptions();
    if (php_rocksdb_array_get_value(vht, "verify_checksums", ztmp))
    {
        rocksdb_container_t->read_options->verify_checksums = zval_is_true(ztmp);
    }
    if (php_rocksdb_array_get_value(vht, "fill_cache", ztmp))
    {
        rocksdb_container_t->read_options->fill_cache = zval_is_true(ztmp);
    }
}

static PHP_METHOD(rocksdb, put)
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key, key_len)
        Z_PARAM_STRING(value, value_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    WriteOptions *wop = rocksdb_container_t->write_options;
    DB *db = rocksdb_container_t->db;

    Status s = db->Put(*wop, std::string(key, key_len), std::string(value, value_len));
    if (!s.ok()) {
        zend_throw_exception(rocksdb_ce, "RocksDB put with read only mode", ROCKSDB_OPEN_ERROR);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb, get)
{
    char *key;
    size_t key_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, key_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    ReadOptions *rop = rocksdb_container_t->read_options;
    DB *db = rocksdb_container_t->db;

    std::string value;
    Status s = db->Get(*rop, std::string(key, key_len), &value);
    if (!s.ok()) {
        RETURN_FALSE;
    }

    RETURN_STRINGL(value.c_str(), value.length());
}

static PHP_METHOD(rocksdb, del)
{
    char *key;
    size_t key_len;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(key, key_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    WriteOptions *wop = rocksdb_container_t->write_options;
    DB *db = rocksdb_container_t->db;
    
    Status s = db->Delete(*wop, std::string(key, key_len));
    if (!s.ok()) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static PHP_METHOD(rocksdb, deleteRange)
{
    char *begin_key;
    size_t begin_key_len;
    char *end_key;
    size_t end_key_len;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(begin_key, begin_key_len)
        Z_PARAM_STRING(end_key, end_key_len)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    rocksdb_container *rocksdb_container_t = php_rocksdb_container_fetch_object(Z_OBJ_P(ZEND_THIS));

    WriteOptions *wop = rocksdb_container_t->write_options;
    DB *db = rocksdb_container_t->db;

    Status s = db->DeleteRange(*wop, 0, std::string(begin_key, begin_key_len), std::string(end_key, end_key_len));
    if (!s.ok()) {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static const zend_function_entry rocksdb_methods[] =
{
    PHP_ME(rocksdb, __construct, arginfo_rocksdb__construct, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, put, arginfo_rocksdb_put, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, get, arginfo_rocksdb_get, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, del, arginfo_rocksdb_del, ZEND_ACC_PUBLIC)
    PHP_ME(rocksdb, deleteRange, arginfo_rocksdb_deleteRange, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(rocksdb)
{
    ROCKSDB_INIT_CLASS_ENTRY(rocksdb, "RocksDB", NULL, NULL, rocksdb_methods);
    ROCKSDB_SET_CLASS_CUSTOM_OBJECT(rocksdb, php_rocksdb_container_create_object, php_rocksdb_container_free_object, rocksdb_container, std);

    php_rocksdb_exception_minit(module_number);

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

