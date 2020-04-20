--TEST--
rocksdb_api: rocksdb iterator
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

$db = new RocksDB('tmp', $option);
$db->put('key1', 'value1');
$db->put('key2', 'value2');
$db->put('key3', 'value3');

$iter = $db->newIterator('key1');
foreach ($iter as $key => $value) {
    var_dump($key);
    var_dump($value);
}
?>
--EXPECT--
string(4) "key1"
string(6) "value1"
string(4) "key2"
string(6) "value2"
string(4) "key3"
string(6) "value3"