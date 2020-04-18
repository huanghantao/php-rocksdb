--TEST--
rocksdb_api: rocksdb put
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$ret = $db->put('key', 'value');
var_dump($ret);
?>
--EXPECT--
bool(true)
