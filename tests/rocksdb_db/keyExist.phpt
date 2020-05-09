--TEST--
rocksdb_db: keyExist
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$path = 'tmp';

$option = [
    'create_if_missing' => true,
];

$db = new RocksDB\DB($path, $option);
$db->open($path, $option);

Assert::false($db->keyExist('nonexistent'));

$db->put('key', 'value');
Assert::true($db->keyExist('key'));

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB($path));
?>
--EXPECT--
