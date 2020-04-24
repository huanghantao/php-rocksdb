--TEST--
rocksdb_transaction_db: snapshot
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

$db = new RocksDB\TransactionDB();
$db->open('tmp', $option);
$db->put('key', 'value');
Assert::eq($db->get('key'), 'value');
$transaction = $db->beginTransaction([], ['set_snapshot' => true]);
$snapshot = $transaction->getSnapshot();
$db->put('key', 'value1');
Assert::eq($db->get('key'), 'value1');
Assert::eq($db->Get("key", ['snapshot' => $snapshot]), 'value');

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
