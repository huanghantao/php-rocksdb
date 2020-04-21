--TEST--
rocksdb_api: rocksdb close
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
Assert::true($db->close());
Assert::true($db->put('key', 'value'));
?>
--EXPECT--
