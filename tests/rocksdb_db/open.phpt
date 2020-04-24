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
?>
--EXPECT--