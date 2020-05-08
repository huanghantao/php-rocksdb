<?php

// PHP settings
error_reporting(E_ALL ^ E_DEPRECATED);
ini_set('memory_limit', '1024M');
assert_options(ASSERT_ACTIVE, 1);
assert_options(ASSERT_WARNING, 1);
assert_options(ASSERT_BAIL, 0);

// Components
require __DIR__ . '/lib/vendor/autoload.php';

class Assert extends RocksdbTest\Assert
{
    protected static $throwException = false;
}
