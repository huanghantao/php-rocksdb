# Introduction

**A PHP extension for Facebook's RocksDB.**

## Example

```php
<?php

$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$db->put('key', 'value');
$db->get('key');
```

```php
<?php

$db = new RocksDB('tmp', ['create_if_missing' => true], [], []);
$db->put('key', 'value');
$db->del('key');
```

```php
<?php

$option = [
    'create_if_missing' => true,
];

$db = new RocksDB('tmp', $option, [], []);
$db->put('key1', 'value1');
$db->put('key2', 'value2');
$db->put('key3', 'value3');
$db->deleteRange('key1', 'key3');
```

```php
<?php

$option = [
    'create_if_missing' => true,
];

$db = new RocksDB('tmp', $option, [], []);
$db->put('key1', 'value1');
$db->put('key2', 'value2');
$db->put('key3', 'value3');

$iter = $db->newIterator('key1');
foreach ($iter as $key => $value) {
}
```

## Installation

### Compiling requirements

1. Linux, OS X or Cygwin, WSL
2. PHP 7.0.0 or later
3. GCC 4.8 or later

### Install from source

```bash
git clone https://github.com/huanghantao/php-rocksdb.git && \
cd php-rocksdb && \
phpize && \
./configure && \
make && make install
```

#### Enable extension in PHP

After compiling and installing to the system successfully, you have to add a new line extension=rocksdb.so to php.ini to enable rocksdb extension.

## Bug

You can feedback through issues, and we will reply within 24 hours.
