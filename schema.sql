CREATE DATABASE `CloudMusic` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci;

USE `CloudMusic`;

CREATE TABLE user (
	id int NOT NULL AUTO_INCREMENT,
	username varchar(32) NOT NULL UNIQUE,
	password varchar(40) NOT NULL,
	PRIMARY KEY(id)
);

CREATE TABLE role (
	id int NOT NULL AUTO_INCREMENT,
	rolename varchar(32) UNIQUE,
	PRIMARY KEY(id)
);

CREATE TABLE auth (
	id int NOT NULL AUTO_INCREMENT,
	authname varchar(32) UNIQUE,
	PRIMARY KEY(id)
);

CREATE TABLE user_role(
	userId int,
	roleId int,
	FOREIGN KEY (userId) REFERENCES user(id),
	FOREIGN KEY (roleId) REFERENCES role(id)
);

CREATE TABLE role_auth(
	roleId int,
	authId int,
	FOREIGN KEY (roleId) REFERENCES role(id),
	FOREIGN KEY (authId) REFERENCES auth(id)
);

CREATE TABLE music(
	id int NOT NULL AUTO_INCREMENT,
	name varchar(30),
	singer varchar(30),
	PRIMARY KEY(id)
);

CREATE TABLE comment(
	id int NOT NULL AUTO_INCREMENT,
	musicId int,
	userId int,
	content longtext,
	thumb int,
	date date,
	PRIMARY KEY (id),
	FOREIGN KEY (musicId) REFERENCES music(id),
	FOREIGN KEY (userId) REFERENCES user(id)
);

CREATE TABLE userMusic(
	userId int,
	musicId int,
	FOREIGN KEY (userId) REFERENCES user(id),
	FOREIGN KEY (musicId) REFERENCES music(id)
);

CREATE TABLE follow(
	followee int,
	follower int,
	FOREIGN KEY (followee) REFERENCES user(id),
	FOREIGN KEY (follower) REFERENCES user(id)
);
