# gateway.poc
Proof of concept of an asynchronous gateway with persistent state.

## Building

### Prerequisites

To compile the project the following is required:
- C++ compiler with C++20 support (tested with GCC and Clang)
- CMake >= 3.20.0

Also the set of third party libraries. This is the list of Ubuntu 22.04 packages:
- libboost-log-dev
- libboost-program-options-dev
- libhiredis-dev
- libpqxx-dev
- postgresql-server-dev-all

Other Linux distros may include the same libraries under different names.

### Compiling

One simple command:
```
./build.h
```
will create a release build. To make a debug one:
```
./build.sh -m debug
```
To delete all build artifacts, including executable binaries:
```
./clean.sh
```


## Supported storages

At the moment 2 storages are supported: PostgreSQL and Redis.

### PostgreSQL

The tested version is 14.5. Steps to install and set up:
- install PostgreSQL:
    ```sh
    sudo apt-get install postgresql
    ```
- create a user and set a password:
    ```sh
    sudo -u postgres createuser -eP <username>
    ```
- create a database:
    ```sh
    sudo -u postgres psql -c 'CREATE DATABASE <dbname>'
    ```
- create messages table:
    ```sh
    sudo -u postgres psql -d <dbname> -c 'CREATE TABLE messages (id BIGSERIAL PRIMARY KEY, ts TIMESTAMP NOT NULL DEFAULT NOW(), data BYTEA NOT NULL)'
    ```
- grant the user right to user the table:
    ```sh
    sudo -u postgres psql -d <dbname> -c 'GRANT SELECT, INSERT, DELETE ON TABLE messages TO "<username>"'
    sudo -u postgres psql -d <dbname> -c 'GRANT USAGE, SELECT ON SEQUENCE messages_id_seq TO "<username>"'
    ```
- add line to the file `/etc/postgresql/14/main/postgresql.conf` to accept connections on all network interfaces:
    ```
    listen_addresses = '*'
    ```
- add lines to the file `/etc/postgresql/14/main/pg_hba.conf` to accept connection from other hosts:
    ```
    host    all     all     0.0.0.0/0   scram-sha-256
    host    all     all     ::/0        scram-sha-256
    ```
- increase maximum number of simultaneous connections in `/etc/postgresql/14/main/postgresql.conf`:
    ```
    max_connections = 512
    ```

### Redis

The tested verion is 7.0. Steps to install and set up:
- add official Redis repository:
    ```sh
    curl -fsSL https://packages.redis.io/gpg | sudo gpg --dearmor -o /usr/share/keyrings/redis-archive-keyring.gpg
    echo "deb [signed-by=/usr/share/keyrings/redis-archive-keyring.gpg] https://packages.redis.io/deb $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/redis.list
    ```
- install Redis:
    ```sh
    sudo apt-get update
    sudo apt-get install redis
    ```
- add the following lines to the `/etc/redis/redis.conf` file:
    - to accept connections on all network interfaces:
        ```
        bind * -::*
        ```
    - to create a user with password:
        ```
        user <username> allcommands allkeys on ><passwrod>
        ```
    - to set up passwrod for the default user:
        ```
        requirepass 12345
        ```
    - to enable Append Only File for maximum durability:
        ```
        appendonly yes
        ```
    - to fsync on every write for maximum durability:
        ```
        appendfsync always
        ```
