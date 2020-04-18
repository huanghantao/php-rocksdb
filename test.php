<?php

$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$ret = $db->put('key', 'value');
$ret = $db->del('key');
$ret = $db->get('key');
var_dump($ret);