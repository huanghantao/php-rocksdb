<?php

$db = new RocksDB\DB();

$path = '/Users/hantaohuang/codeDir/talCode/naming-gateway/rocksdb/service/';

$db->open($path);

$ret = [];

$iter = $db->newIterator("/e");
foreach ($iter as $key => $value) {
    var_dump($key, $value);
}