--TEST--
rocksdb_api: rocksdb del
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
Assert::true($db->del('key'));

try {
    $db->get('key');
} catch (RocksDB\Exception $e) {
    Assert::eq($e->getMessage(), "NotFound: ");
}
?>
--EXPECT--