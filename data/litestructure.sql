CREATE TABLE author_list (recipe_id INTEGER NOT NULL,author_id INTEGER NOT NULL);
CREATE TABLE authors (id INTEGER NOT NULL, name varchar(20) default NULL,PRIMARY KEY (id));
CREATE TABLE categories (id INTEGER NOT NULL, name varchar(20) default NULL,PRIMARY KEY (id));
CREATE TABLE category_list (recipe_id INTEGER NOT NULL,category_id INTEGER NOT NULL);
CREATE TABLE db_info (ver FLOAT NOT NULL,generated_by varchar(200) default NULL);
CREATE TABLE ingredient_info (ingredient_id INTEGER, property_id INTEGER, amount FLOAT, per_units INTEGER);
CREATE TABLE ingredient_list (recipe_id INTEGER, ingredient_id INTEGER, amount FLOAT, unit_id INTEGER, order_index INTEGER);
CREATE TABLE ingredient_properties (id INTEGER NOT NULL,name VARCHAR(20), units VARCHAR(20), PRIMARY KEY (id));
CREATE TABLE ingredients (id INTEGER NOT NULL, name VARCHAR(50), PRIMARY KEY (id));
CREATE TABLE recipes (id INTEGER NOT NULL,title VARCHAR(200),persons INTEGER,instructions TEXT, photo BLOB,   PRIMARY KEY (id));
CREATE TABLE unit_list (ingredient_id INTEGER, unit_id INTEGER);
CREATE TABLE units (id INTEGER NOT NULL, name VARCHAR(20), PRIMARY KEY (id));
CREATE TABLE units_conversion (unit1_id INTEGER, unit2_id INTEGER, ratio FLOAT);
CREATE index cid_index ON category_list(category_id);
CREATE index iidil_index ON ingredient_list(ingredient_id);
CREATE index rid_index ON category_list(recipe_id);
CREATE index ridil_index ON ingredient_list(recipe_id);

INSERT INTO db_info VALUES(0.4,'Krecipes 0.4');