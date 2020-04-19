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
try {
    $db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
    $db->put('key', 'value');
    $db->del('key');
    $db->get('key');
} catch (RocksDB\Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(10) "NotFound: "