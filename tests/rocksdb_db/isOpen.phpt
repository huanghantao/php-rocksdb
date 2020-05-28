--TEST--
rocksdb_db: check rocksdb is open
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
Assert::false($db->isOpen());
$db->open($path, $option);
Assert::true($db->isOpen());

Assert::true($db->close());
Assert::false($db->isOpen());
Assert::true(RocksDB\DB::destroyDB($path));
?>
--EXPECT--
