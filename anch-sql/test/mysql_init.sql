CREATE DATABASE IF NOT EXISTS anch_test;
CREATE USER 'anch'@'localhost' IDENTIFIED BY 'anch';
GRANT ALL PRIVILEGES ON anch_test.* TO 'anch'@'localhost';
FLUSH PRIVILEGES;

USE anch_test;

CREATE TABLE IF NOT EXISTS  T_Test (
       id INTEGER NOT NULL AUTO_INCREMENT,
       first_name VARCHAR(256) NOT NULL DEFAULT 'John',
       last_name VARCHAR(256) NOT NULL DEFAULT 'Doe',
       birth_date DATE NOT NULL,
       email VARCHAR(512),
       PRIMARY KEY (id)
);
CREATE INDEX T_Test_name ON T_Test (first_name, last_name) USING BTREE;

INSERT INTO T_Test (birth_date) VALUES ('1970-01-01');
INSERT INTO T_Test (first_name, last_name, birth_date,email) VALUES ('George', 'Abitbol', '1993-01-01','george.abitbol@classe.us');
INSERT INTO T_Test (first_name, last_name, birth_date) VALUES ('Jesus', 'Christ', '0000-12-24');
INSERT INTO T_Test (first_name, last_name, birth_date) VALUES ('MySQL', 'MySQL', '2016-03-08');
