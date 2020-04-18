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
$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$db->put('key', 'value');
$ret = $db->get('key');
var_dump($ret);
?>
--EXPECT--
string(5) "value"
