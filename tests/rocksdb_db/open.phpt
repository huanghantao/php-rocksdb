--TEST--
rocksdb_db: rocksdb open
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$db = new RocksDB\DB();
Assert::isInstanceOf($db, RocksDB\DB::class);

Assert::true($db->open('tmp', ['create_if_missing' => true]));
Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--