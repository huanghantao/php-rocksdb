--TEST--
rocksdb_db: open rocksdb read only
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
try {
    $db->put('key1', 'value1');

} catch (RocksDB\Exception $e) {
	Assert::eq($e->getMessage(), 'Not implemented: Not supported operation in read only mode.');
}

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
