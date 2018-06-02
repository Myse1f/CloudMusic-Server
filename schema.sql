CREATE DATABASE `CloudMusic` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `CloudMusic`;

CREATE TABLE user (
	id int NOT NULL AUTO_INCREMENT,
	username varchar(32) NOT NULL,
	password varchar(40) NOT NULL,
	PRIMARY KEY(id)
);

CREATE TABLE role (
	id int NOT NULL AUTO_INCREMENT,
	rolename varchar(32),
	PRIMARY KEY(id)
);

CREATE TABLE auth (
	id int NOT NULL AUTO_INCREMENT,
	authname varchar(32),
	PRIMARY KEY(id)
);

CREATE TABLE user_role();
CREATE TABLE role_auth();

CREATE TABLE music(

);

CREATE TABLE comment(

);

CREATE TABLE userlist(

);
