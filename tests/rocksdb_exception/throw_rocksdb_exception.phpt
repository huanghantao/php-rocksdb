--TEST--
rocksdb_exception: throw RocksDB\Exception
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
try {
    throw new RocksDB\Exception("RocksDB exception", 1);
} catch (RocksDB\Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(17) "RocksDB exception"
