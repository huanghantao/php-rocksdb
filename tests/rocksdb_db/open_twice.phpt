--TEST--
rocksdb_db: open rocksdb twice
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
Assert::true($db->open($path, $option));
Assert::true($db->open($path, $option));

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB($path));
?>
--EXPECT--
