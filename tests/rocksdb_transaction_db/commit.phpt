--TEST--
rocksdb_transaction_db: commit
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

$db = new RocksDB\TransactionDB('tmp', $option);
Assert::true($db->put('key', 'value'));
Assert::eq($db->get('key'), 'value');
$transaction = $db->beginTransaction([], ['set_snapshot' => true]);
Assert::true($transaction->put('key', 'value1'));
Assert::eq($transaction->get('key'), 'value1');
Assert::eq($db->get('key'), 'value');
Assert::true($transaction->commit());
Assert::eq($db->get('key'), 'value1');

Assert::true($db->close());
Assert::true(RocksDB\DB::destroyDB('tmp'));
?>
--EXPECT--
