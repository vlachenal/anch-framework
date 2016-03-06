CREATE TABLE T_Test (
       id SERIAL PRIMARY KEY,
       first_name VARCHAR(256) NOT NULL DEFAULT 'John',
       last_name VARCHAR(256) NOT NULL DEFAULT 'Doe',
       birth_date DATE NOT NULL,
       email VARCHAR(512)
);
CREATE INDEX T_Test_name ON T_Test USING BTREE (first_name, last_name);

INSERT INTO T_Test (birth_date) VALUES ('1970-01-01');
INSERT INTO T_Test (first_name, last_name, birth_date, email) VALUES ('George', 'Abitbol', '1993-01-01','george.abitbol@classe.us');
INSERT INTO T_Test (first_name, last_name, birth_date) VALUES ('Jesus', 'Christ', '0001-12-24');
