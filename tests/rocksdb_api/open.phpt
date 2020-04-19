--TEST--
rocksdb_api: rocksdb open
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

Assert::isInstanceOf(new RocksDB('tmp', ['create_if_missing' => true], [], []), RocksDB::class);
?>
--EXPECT--