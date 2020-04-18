--TEST--
rocksdb_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = rocksdb_test1();

var_dump($ret);
?>
--EXPECT--
The extension rocksdb is loaded and working!
NULL
