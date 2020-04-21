--TEST--
rocksdb_api: rocksdb put
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$db = new RocksDB('tmp', ['create_if_missing' => true]);
Assert::true($db->put('key', 'value'));

Assert::true($db->close());
Assert::true(RocksDB::destroyDB('tmp'));
?>
--EXPECT--
