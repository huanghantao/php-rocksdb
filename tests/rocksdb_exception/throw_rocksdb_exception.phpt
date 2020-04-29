--TEST--
rocksdb_exception: throw RocksDB\Exception
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

try {
    throw new RocksDB\Exception("RocksDB exception", 1);
} catch (RocksDB\Exception $e) {
    Assert::eq($e->getMessage(), 'RocksDB exception');
}
?>
--EXPECT--