--TEST--
Check if rocksdb is loaded
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "rocksdb" is available';
?>
--EXPECT--
The extension "rocksdb" is available
