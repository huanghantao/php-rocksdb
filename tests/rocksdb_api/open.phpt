--TEST--
rocksdb_api: rocksdb open
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
var_dump($db);
?>
--EXPECT--
object(RocksDB)#1 (0) {
}
