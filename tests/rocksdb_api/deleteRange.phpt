--TEST--
rocksdb_api: rocksdb deleteRange
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
$option = [
    'create_if_missing' => true,
];

$db = new RocksDB('tmp', $option, [], []);
$db->put('key1', 'value1');
$db->put('key2', 'value2');
$db->put('key3', 'value3');

$db->deleteRange('key1', 'key3');

$ret = $db->get('key1');
var_dump($ret);
$ret = $db->get('key2');
var_dump($ret);
$ret = $db->get('key3');
var_dump($ret);
?>
--EXPECT--
bool(false)
bool(false)
string(6) "value3"
