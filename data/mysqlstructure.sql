CREATE TABLE author_list (
  recipe_id int(11) NOT NULL default '0',
  author_id int(11) NOT NULL default '0'
) TYPE=MyISAM;
CREATE TABLE authors (
  id int(11) NOT NULL auto_increment,
  name varchar(50) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE categories (
  id int(11) NOT NULL auto_increment,
  name varchar(40) default NULL,
  parent_id int(11) NOT NULL default '-1',
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE category_list (
  recipe_id int(11) NOT NULL default '0',
  category_id int(11) NOT NULL default '0',
  INDEX  rid_index (recipe_id),
  INDEX cid_index (category_id)
) TYPE=MyISAM;
CREATE TABLE db_info (
  ver float NOT NULL default '0',
  generated_by varchar(200) default NULL
) TYPE=MyISAM;
CREATE TABLE ingredient_info (
  ingredient_id int(11) default NULL,
  property_id int(11) default NULL,
  amount float default NULL,
  per_units int(11) default NULL
) TYPE=MyISAM;
CREATE TABLE ingredient_list (
  recipe_id int(11) default NULL,
  ingredient_id int(11) default NULL,
  amount float default NULL,
  unit_id int(11) default NULL,
  prep_method_id int(11) default NULL,
  order_index int(11) default NULL,
  INDEX  ridil_index (recipe_id),
  INDEX iidil_index (ingredient_id)
) TYPE=MyISAM;
CREATE TABLE ingredient_properties (
  id int(11) NOT NULL auto_increment,
  name varchar(20) default NULL,
  units varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE ingredients (
  id int(11) NOT NULL auto_increment,
  name varchar(50) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE recipes (
  id int(11) NOT NULL auto_increment,
  title varchar(200) default NULL,
  persons int(11) default NULL,
  instructions text,
  photo blob,
  prep_time time,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE unit_list (
  ingredient_id int(11) default NULL,
  unit_id int(11) default NULL
) TYPE=MyISAM;
CREATE TABLE units (
  id int(11) NOT NULL auto_increment,
  name varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
CREATE TABLE units_conversion (
  unit1_id int(11) default NULL,
  unit2_id int(11) default NULL,
  ratio float default NULL
) TYPE=MyISAM;
CREATE TABLE prep_methods (
  id int(11) NOT NULL auto_increment,
  name varchar(20) default NULL,
  PRIMARY KEY  (id)
) TYPE=MyISAM;
INSERT INTO db_info VALUES(0.6,'Krecipes 0.6');