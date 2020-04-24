--TEST--
rocksdb_db: rocksdb iterator
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

$db = new RocksDB\DB();
$db->open('tmp', $option);
$db->put('key1', 'value1');
$db->put('key2', 'value2');
$db->put('key3', 'value3');

$iter = $db->newIterator('key1');
foreach ($iter as $key => $value) {
    var_dump($key);
    var_dump($value);
}

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
string(4) "key1"
string(6) "value1"
string(4) "key2"
string(6) "value2"
string(4) "key3"
string(6) "value3"