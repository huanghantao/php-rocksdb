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

$ret = [];

$iter = $db->newIterator('key1');
$i = 1;
foreach ($iter as $key => $value) {
    Assert::eq($key, "key{$i}");
    Assert::eq($value, "value{$i}");
    $i++;
}

unset($iter);

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--