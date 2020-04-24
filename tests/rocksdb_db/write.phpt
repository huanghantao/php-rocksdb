--TEST--
rocksdb_db: rocksdb write
--SKIPIF--
<?php
if (!extension_loaded('rocksdb')) {
	echo 'skip';
}
?>
--FILE--
<?php
require __DIR__ . '/../include/bootstrap.php';

$db = new RocksDB\DB();
$db->open('tmp', ['create_if_missing' => true]);

$batch = new RocksDB\WriteBatch();
$batch->put('key', 'value');
$batch->delete('key');

$db->write($batch);

try {
    $db->get('key');
} catch (RocksDB\Exception $e) {
    Assert::eq($e->getMessage(), "NotFound: ");
}

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
