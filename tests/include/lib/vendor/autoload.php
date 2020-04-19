<?php
spl_autoload_register(function (string $classname) {
    $classname = explode('\\', $classname);
    $namespace = array_shift($classname);
    $path = implode('/', $classname);
    switch ($namespace) {
        case 'RocksdbTest':
            require __DIR__ . "/../src/{$path}.php";
            break;
    }
});
