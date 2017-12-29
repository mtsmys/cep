# cep
C library for CEP(Complex Event Processing) in embedded system

## Features
This library uses SQLite 3 as an external shared library.<br>
Therefore, libsqlite3 is required when operating on the target device.

In CEP, in order to reduce the overhead of data processing, we build a database in memory space using SQLite and process data on this memory database.<br>
For data stored in the memory database, if it reaches the arbitrarily set number of records, it will be swept out from the first record. <br>
As a result, the record acquired by SQL for data processing is only the record that is currently focused.

Records swept from the memory database can be saved in the file database for redundancy.

## Installation
Build is OK with make.<br>
Installation is meke install.

```
$ make
$ make install
```

However, since the current Makefile is set for Raspberry Pi, please edit the Makefile when installing with other OS.
