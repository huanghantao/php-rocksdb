--TEST--
rocksdb_db: rocksdb put
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
$db->open('tmp', ['create_if_missing' => true]);
Assert::true($db->put('key', 'value'));

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
