#!/bin/sh
# Start MariaDB and PostgreSQL in the CI container (no systemd) and create anch-sql test databases.
# Tests connect to localhost: MariaDB through /run/mysqld/mysqld.sock, PostgreSQL through TCP.
# Usage: start-databases.sh <repository root>
set -eu

root=$(realpath "${1:-.}")

# MariaDB +
mkdir -p /run/mysqld
chown mysql:mysql /run/mysqld
mariadb-install-db --user=mysql --basedir=/usr --datadir=/var/lib/mysql > /dev/null
nohup mariadbd-safe --user=mysql --datadir=/var/lib/mysql > /tmp/mariadb.log 2>&1 &
for _ in $(seq 60); do
  mariadb-admin ping --silent 2> /dev/null && break
  sleep 1
done
mariadb-admin ping
mariadb -u root <<'SQL'
CREATE DATABASE anch_test;
CREATE USER 'anch'@'localhost' IDENTIFIED BY 'anch';
GRANT ALL PRIVILEGES ON anch_test.* TO 'anch'@'localhost';
SQL
mariadb -u anch -panch anch_test < "${root}/anch-sql/test/mysql_init.sql"
# MariaDB -

# PostgreSQL +
# postgres account is expired in Arch Linux image: su (PAM) refuses it, setpriv does not use PAM
as_postgres() {
  (cd /tmp && setpriv --reuid=postgres --regid=postgres --init-groups "$@")
}
mkdir -p /run/postgresql
chown postgres:postgres /run/postgresql
as_postgres initdb -D /var/lib/postgres/data -E UTF8 --locale=C --auth-local=trust --auth-host=scram-sha-256 > /dev/null
as_postgres pg_ctl -D /var/lib/postgres/data -l /tmp/postgresql.log -w start
as_postgres psql -q -c "CREATE USER anch PASSWORD 'anch'"
as_postgres createdb -O anch anch_test
PGPASSWORD=anch psql -q -h localhost -U anch -d anch_test -v ON_ERROR_STOP=1 -f "${root}/anch-sql/test/postgresql_init.sql"
# PostgreSQL -
