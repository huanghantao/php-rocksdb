--TEST--
rocksdb_db: rocksdb deleteRange
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$option = [
    'create_if_missing' => true,
];

$db = new RocksDB\DB('tmp', $option);
Assert::true($db->put('key1', 'value1'));
Assert::true($db->put('key2', 'value2'));
Assert::true($db->put('key3', 'value3'));
Assert::true($db->deleteRange('key1', 'key3'));

Assert::eq($db->get('key3'), 'value3');

try {
    $ret = $db->get('key1');
    $ret = $db->get('key2');
} catch (RocksDB\Exception $e) {
    Assert::eq($e->getMessage(), "NotFound: ");
}

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
