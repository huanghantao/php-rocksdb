--TEST--
rocksdb_api: rocksdb get
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
$db->put('key', 'value');
Assert::eq($db->get('key'), 'value');

Assert::true($db->close());
Assert::true(RocksDB::destroyDB('tmp'));
?>
--EXPECT--
