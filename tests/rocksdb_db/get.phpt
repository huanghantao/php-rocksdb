--TEST--
rocksdb_db: rocksdb get
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$db = new RocksDB\DB('tmp', ['create_if_missing' => true]);
$db->put('key', 'value');
Assert::eq($db->get('key'), 'value');

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
