<?php

$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$ret = $db->put('key', 'value');
var_dump($ret);
$ret = $db->get('key');
var_dump($ret);

$ret = $db->del('key');
var_dump($ret);

$ret = $db->get('key');
var_dump($ret);

$ret = $db->put('key', 'value');
var_dump($ret);
$ret = $db->get('key');
var_dump($ret);