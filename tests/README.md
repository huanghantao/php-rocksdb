# PHP Unit-test

Run these tests to make certain that the rocksdb extension you installed can work well.

> The testing system was migrated from Swoole.

## How to run

1. just run `./start.sh api`

## Defaults

| Config    | Enable   |
| --------- | -------- |
| show-diff | yes      |
| show-mem  | yes      |
| show-slow | 1000(ms) |

## Log files

| suffix | intro                                         |
| ------ | --------------------------------------------- |
| diff   | show the differents between output and expect |
| out    | script output                                 |
| exp    | expect output                                 |
| log    | all above                                     |
| php    | php temp script file                          |

## Clean

Run `./clean` to remove all of the tests log files.

## Contribute the test script

Run `./new [test-script-filename]`

E.g. : `./new ./rocksdb_db/test.phpt`

It will generate the test script file and auto open on your ide (MacOS only).

![](https://ws1.sinaimg.cn/large/006DQdzWly1frvn56azn9g30rs0m8b29.gif)
